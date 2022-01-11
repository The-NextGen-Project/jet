#include "parser.h"


using namespace nextgen;
using namespace nextgen::jet;

using nextgen::jet::Parser;
using nextgen::jet::Parse_Error;
using nextgen::jet::Parse_Error_Kind;
using nextgen::jet::Parse_Context;
using nextgen::jet::Token;

auto parse_expr(Parser &P, int previous_binding = -1) -> const Syntax_Node*;
auto match_expr(Parser &P) -> const Syntax_Node *;
auto parse_block(Parser &P, bool ret = false) -> Syntax_Block;
auto parse_stmt(Parser &P) -> const Syntax_Node*;

static auto UnaryOperatorBinding(Token_Kind kind) {
  switch (kind) {
    case Token_Kind::Plus:
    case Token_Kind::Minus:
    case Token_Kind::Exclamation_Point:
    case Token_Kind::NOT:
      return 17;
    default:
      return -1;
  }
}

static auto InfixOperatorBinding(Token_Kind kind) -> std::array<int, 2>{
  switch (kind) {
    case Token_Kind::Dot:
      return {16, 15}; // Property value operator
    case Token_Kind::Star:
    case Token_Kind::Slash:
    case Token_Kind::Percent:
      return {13, 14};
    case Token_Kind::Plus:
    case Token_Kind::Minus:
      return {11, 12};
    case Token_Kind::Greater_Than:
    case Token_Kind::Less_Than:
      return {9, 10};
    case Token_Kind::Equals_Equals:
      return {7, 8};
    case Token_Kind::Keyword_And:
      return {5, 6};
    case Token_Kind::Keyword_Or:
      return {3, 4};
    case Token_Kind::AND:
    case Token_Kind::NOT:
    case Token_Kind::XOR:
    case Token_Kind::Pipe:
      return {1, 2};
    default:
      return {-1, -1};
  }
}


auto parse_expr(Parser &P, int previous_binding) -> const Syntax_Node* {
  Syntax_Node *lhs;

  auto first_token = P.curr();
  auto first_kind = first_token->type();

  // ~ : Bitwise NOT
  // + : Positive
  // - : Negation
  // & : allocator Reference
  // * : pointer Dereference
  // not : Logical Negation
  auto unary_binding = UnaryOperatorBinding(first_kind);
  if (unary_binding > previous_binding) {
    const auto expr  = parse_expr(P, unary_binding);
    const auto op    = Syntax_Unary::MatchOp(first_kind);
    return new Syntax_Unary(op, first_token, expr);
  } else {
    lhs = const_cast<Syntax_Node*>(match_expr(P));
  }
  (void) P.next(1U);
  do {
    auto op = P.curr();
    auto op_kind = op->type();

    const auto op_bindings = InfixOperatorBinding(op_kind);
    if (/* LeftBindingPrecedence */ op_bindings[0] <= previous_binding or op_bindings[0] is -1) {
      break;
    }
    (void) P.skip(1U);
    const auto rhs = parse_expr(P, /* RightBindingPrecedence */op_bindings[1]);
    auto new_expr = new Syntax_Binary {
      Syntax_Binary::MatchOp(op_kind), op, lhs, rhs
    };
    lhs = new_expr;
  } while (P.curr()->is_valid_expression_type());
  return lhs;
}

auto parse_type(Parser &P) -> Syntax_Type* {

  auto check_for_next_type = [&P](auto annotation) {
    auto typename_token = P.curr();
    if (typename_token->is_valid_typename()) {
      return new Syntax_Type(annotation, Syntax_Type::MatchTypename(typename_token), nullptr);
    }
    else if (typename_token->is_valid_typename_start()) {
      auto parsed_type = parse_type(P);
      return new Syntax_Type(annotation, None, parsed_type);
    }
    else {
      // TODO: Add Typename Error Here
      return (Syntax_Type*) nullptr;
    }
  };

  auto first_token = P.curr();
  switch(first_token->type()) {

    case LBracket: {
      // TODO: Add ability for finite array size
      (void) P.expect<RBracket>("Sequence '[]' (representing array) must be met with closing value ']'");
      return check_for_next_type(Syntax_Type_Annotation::ArrayType);
    }
    case LParenthesis: {
      // TODO: Add tuples
      break;
    }
    case Star: {
      P.next(1U);
      return check_for_next_type(Syntax_Type_Annotation::Pointer);
    }
    default:
      UNREACHABLE;
  }
  return new Syntax_Type(None, None, nullptr);
}

auto parse_variable_assignment(Parser &P, const Token *name)  {

  // Check 1: name: type = expr
  // Check 2: name := expr
  if (P.peek(1U)->type() is Token_Kind::Colon) {
    auto E = new Syntax_Variable_Assignment(name, Some(parse_type(P)), parse_expr(P));
    P.expect<Token_Kind::Semi_Colon>("Expected ';' after declaration");
    return E;
  } else {
    auto E = new Syntax_Variable_Assignment(name, None, parse_expr(P));
    P.expect<Token_Kind::Semi_Colon>("Expected ';' after declaration");
    return E;
  }
}

auto parse_variable_value_assignment(Parser &P, const Token *name, Syntax_Assignment_Op op)  {
  auto ret = new Syntax_Variable_Value_Assignment(name, parse_expr(P), op);
  (void)P.expect<Token_Kind::Semi_Colon>("Expected ';' after declaration");
  return ret;
}

auto parse_while(Parser &P) -> const Syntax_Node *{
  auto cond = parse_expr(P);
  auto body = parse_block(P);
  return new Syntax_While(cond, body);
}

auto parse_for(Parser &P) -> const Syntax_Node * {
  auto loop_variable = P.skip<Identifier, Missing_For_Loop_Variable>();
  P.skip<Keyword_In, Expected_Token>();
  auto list_or_range = P.skip(1);
  switch (list_or_range->type()) {
    case Identifier: {
      auto body = parse_block(P);
      return new Syntax_ForList(list_or_range, loop_variable, body);
    }
    case Keyword_Range: {
      P.expect<Token_Kind::LParenthesis>("");
      Range_Function func;
      FOR(value, 3) {
        if (P.curr()->type() is RParenthesis or P.curr()->type() is EOFToken)
          break;
        func.set[value] = P.expect<Token_Kind::Integer>("Range function values "
                                                        "must "
                                                        "be "
                                                        "integers")->get_value<size_t>();
        P.expect<Comma>("Expected comma in between parameters.");
      }
      auto body = parse_block(P);
      return new Syntax_ForRange(loop_variable, func, body);
    }
    default:
      P.diagnostics.build(Parse_Error {
        Parse_Error_Kind::Invalid_Token,
        list_or_range->location(),
        {Parse_Error::Metadata("Invalid token in for-loop context. Expected "
                               "a list variable or the range function.") }
      });
      break;
  }
  return nullptr;
}

template<bool ELIF = false>
auto parse_if(Parser &P) -> const Syntax_Node* {
  auto cond = parse_expr(P);
  auto body = parse_block(P);

  Syntax_Else *else_ = nullptr;
  Syntax_Elif *elif = nullptr;

  auto is_else = P.next(1);
  if (is_else->type() == Token_Kind::Keyword_Else) {
    else_ = new Syntax_Else(parse_block(P));
  }
  else if (is_else->type() == Token_Kind::Keyword_Elif) {
    elif = (Syntax_Elif*) parse_if<true>(P);
  }

  if constexpr (ELIF) {
    return new Syntax_Elif(body, elif);
  } else {
    return new Syntax_If(cond, body, else_, elif);
  }
}


auto parse_stmt(Parser &P) -> const Syntax_Node* {
  auto C1 = P.peek(1);
  switch (P.curr()->type()) {
    case Keyword_If:
      P.skip(1);
      return parse_if(P);
    case Keyword_While:
      P.context = Parse_Context::Loop_Block;
      P.skip(1);
      return parse_while(P);
    case Keyword_For:
      P.context = Parse_Context::Loop_Block;
      P.skip(1);
      return parse_for(P);
    case Keyword_Break:
      if (P.context != Parse_Context::Loop_Block) {
        // TODO: Add Error
      } else {
        return new struct Break();
      }
    case Keyword_Continue:
      if (P.context != Parse_Context::Loop_Block) {
        // TODO: Add Error
      } else {
        return new struct Continue();
      }
    case Keyword_Defer:
      return new struct Defer(parse_expr(P));
    case Keyword_Return:
      return new Return(parse_expr(P));
    case Keyword_Match:
      break;
    case Identifier: {
      if (C1->type() is Colon_Equals) {
        auto var_name = P.curr();
        P.skip(2);
        return parse_variable_assignment(P, var_name);
      }
      if (C1->is_value_assignment_op()) {
        auto var_name = P.curr();
        P.skip(2);
        return parse_variable_value_assignment(P, var_name,
                                               Syntax_Variable_Value_Assignment::MatchOp(C1->type()));
      }
    }
    default: break;
  }
  P.next(1);
  return nullptr;
}

auto parse_function_param(Parser &P) {
  std::vector<Syntax_Function_Parameter> params;
  params.reserve(4U);

  P.skip<Token_Kind::LParenthesis, Parse_Error_Kind::Expected_Token>();
  while (P.curr_not_eq<RParenthesis>()) {
    auto parameter_name = P.skip<Identifier, Expected_Identifier_For_Function_Parameter>();
    P.expect<Colon>("Expected ':' before parameter type");
    params.emplace_back(Syntax_Function_Parameter(parameter_name, parse_type(P)));
  }
  return params;
}


auto parse_function_call(Parser &P, const Token *function_name, const Token *delim) {
  auto E = new Syntax_Function_Call(function_name);
  while (P.curr_not_eq<RParenthesis>()) {
    E->parameters.emplace_back(parse_expr(P));
  }
  P.expect_delim<Token_Kind::RParenthesis>(delim->location());
  return E;
}

auto parse_block(Parser &P, bool ret) -> Syntax_Block  {
  P.expect<LCurly_Brace>("Expected '{' before beginning of current");
  auto block   = Syntax_Block {};
  while (P.curr_not_eq<RCurly_Brace>()) {
    auto stmt = parse_stmt(P);
    block.statements.emplace_back(stmt);
    if (ret) {
      if (stmt->kind is Syntax_Kind::Return_Value)
        block.returns.emplace_back(stmt);
    }
  }
  P.expect_delim<RCurly_Brace>(P.curr()->location());
  P.next(1U);
  return block;
}

auto parse_function(Parser &P, const Token *function_name) {
  auto type   = parse_type(P);
  auto block  = parse_block(P);
  auto params = parse_function_param(P);
  return new Syntax_Function(function_name, type, block, params);
}

auto parse_struct_data_members(Parser &P) {
  return ALLOC_IF(Syntax_Struct_Member, P.curr_not_eq<RCurly_Brace>(), {
    auto member_name = P.skip<Identifier, Expected_Identifier_For_Function_Parameter>();
    P.expect<Colon>("Expected ':' before parameter type");
    _ = new Syntax_Struct_Member(member_name, parse_type(P));
  });
}


auto parse_struct(Parser &P, const Token *name) {
  P.expect<LCurly_Brace>("Opening curly brace is required before struct member declarations");
  const auto struct_value = new Syntax_Struct(name, parse_struct_data_members(P));
  P.expect<RCurly_Brace>("Missing closing '}' after struct declaration");
  return struct_value;
}

auto parse_struct_instantiation(Parser &P, const Token *name)  {
  auto values = ALLOC_IF(Syntax_Struct_Member_Initialization, P.curr_not_eq<RCurly_Brace>(), {
    auto struct_name = P.expect<Identifier>("Identifiers must be used for struct member name");
    P.expect<Colon>("Expected ':' before struct member name");
    auto value = parse_expr(P);
    _ = new Syntax_Struct_Member_Initialization(struct_name, value);
  });

  P.expect_delim<Token_Kind::RCurly_Brace>(P.curr()->location());
  return new Syntax_Struct_Instantiation(name,values);
}

auto parse_enum(Parser &P, const Token *name) {
  auto enumeration = new Syntax_Enum;
  enumeration->name = name;
  while (P.curr_not_eq<RCurly_Brace>()) {
    auto constant = P.expect<Identifier>("Identifiers must be used for enum value");
    if (P.curr()->type() is Comma) {
      P.skip(1);
    }
    else if (P.curr()->type() is LCurly_Brace) {
      P.skip(2);
      enumeration->enumeration_values.emplace_back(Syntax_Enum::Value(parse_struct(P, constant)));
    }
    else {
      if (P.curr()->type() != RCurly_Brace) {
        // TODO: Add Error Here
      } else {
        enumeration->enumeration_values.emplace_back(Syntax_Enum::Value(constant));
      }
    }
  }
  P.expect_delim<RCurly_Brace>(P.curr()->location());
  return enumeration;
}

auto parse_list_initialization(Parser &P, const Token *matched_token) -> const Syntax_Node * {
  auto current = P.next(1);
  auto E  = new Syntax_List;

  while (current->type() != Token_Kind::RBracket and current->type() != Token_Kind::EOFToken) {
    auto list_value_expression = parse_expr(P);
    E->values.emplace_back(list_value_expression);

    current = P.curr();

    if(current->type() is Token_Kind::Comma)
      P.skip(1);
    else if (current->type() is Token_Kind::RBracket)
      break;
    else
      P.expect<Token_Kind::Comma>("Expected ',' or ']' here");

  }
  P.expect_delim<Token_Kind::RBracket>(matched_token->location());
  P.position--;
  auto node = (Syntax_Node*)E;
  node->kind = Syntax_Kind::List;
  return node;
}

auto match_expr(Parser& P) -> const Syntax_Node * {
  auto matched_token = P.curr();
  switch (matched_token->type()) {

    // We have reached the EOF somehow, build an error for it here
    case EOFToken:
      P.diagnostics.build(Parse_Error {
        Parse_Error_Kind::Unexpected_End_Of_File,
        matched_token->location()
      });
      break;

      // Two Cases:
      // 1. A Variable  ... = my_ident_which_is_var
      // 2. A Function Call  ... nice_func(1, 2)
    case Identifier: {
      auto C1 = P.peek(1);
      if (C1->type() is Token_Kind::LParenthesis) {
        return parse_function_call(P, matched_token, C1);
      }
      else if (C1->type() is Token_Kind::LCurly_Brace) {
        // We are maybe in a struct instantiation
        auto cond1 = P.peek(2);
        auto cond2 = P.peek(3);
        if (cond1 and cond2 and cond1->type() is Identifier and cond2->type() is Colon) {
          return parse_struct_instantiation(P, matched_token);
        }
      }
      else if (C1->type() is Token_Kind::Path) {
        // We are in a path value (ie: SomeEnum::Value)
//        return P.parse_path(matched_token);
      }
      auto E = (Syntax_Node*) new Syntax_Literal(matched_token);
      E->kind = Syntax_Kind::Literal_Value;
      return E;
    }

      // Constant Value.
    case Integer: case String: case Char: case Decimal: case Keyword_True:
    case Keyword_False: {
      auto E = (Syntax_Node*) new Syntax_Literal(matched_token);
      E->kind = Syntax_Kind::Literal_Value;
      return E;
    }

      // Parenthesized Expression
    case LParenthesis: {
      P.skip(1);
      auto E = parse_expr(P);
      P.expect_delim<Token_Kind::RParenthesis>(P.curr()->location());
      P.position--;
      return E;
    }
    case LBracket: return parse_list_initialization(P, matched_token);
    default: // TODO: Unexpected Expression Token
      break;
  }
  return nullptr;
}


auto parse(Parser &P) -> Parse_Result {
  auto output = Parse_Result{};

  for (;;) {

    // Report up to 2 errors before exiting the program
    if (P.fatal >= 2) {
      P.diagnostics.send_exception();
      break;
    }

    switch (P.curr()->type()) {
      case Token_Kind::EOFToken: return output;
      case Token_Kind::Identifier: {
        auto C1 = P.peek(1U);
        auto C2 = P.peek(2U);
        switch (C1->type()) {
          case Token_Kind::Path: { // func ::
            switch (C2->type()) {
              case Token_Kind::LParenthesis:
                (void)output.functions.emplace_back(parse_function(P, P.curr())); break;
              case Token_Kind::Keyword_Struct:
                (void)output.structures.emplace_back(parse_struct(P, P.curr())); break;
              case Token_Kind::Keyword_Enum:
                (void)output.enumerations.emplace_back(parse_enum(P, P.curr())); break;
              default: UNREACHABLE;
            }
          }
          case Token_Kind::Colon_Equals: {
            output.global_variables
              .emplace_back((const Syntax_Variable_Assignment*) parse_variable_assignment(P, P.curr()));
            break;
          }
          default: {
            P.diagnostics.build(Parse_Error {
              Parse_Error_Kind::Invalid_Token_After_Ident_In_Global_Scope,
              C1->location(),
              {Parse_Error::Metadata {C1}}
            });
            (void)P.diagnostics.send_exception();
          }
        }
        break;
      }
      default: // TODO: Figure out what to do here
        break;
    }
  }
  return output;
}
