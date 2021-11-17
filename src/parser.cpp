#include "parser.h"
#include "allocator.h"


using namespace nextgen::jet;

const Parse_Result
Parser::parse() {
  Parse_Result output;

  for (;;) {

    // Report up to 2 errors before exiting the program
    if (fatal >= 2) {
      this->diagnostics.send_exception();
    }
    
    switch (curr()->type()) {
      case Token_Kind::EOFToken: return output;
      case Token_Kind::Identifier: {
        auto C1 = peek(1);
        auto C2 = peek(2);
        switch (C1->type()) {
          case Token_Kind::Path: { // func ::
            switch (C2->type()) {
              case Token_Kind::LParenthesis:
                output.functions.emplace_back(parse_function(curr())); break;
              case Token_Kind::Keyword_Struct:
                output.structures.emplace_back(parse_struct(curr())); break;
              case Token_Kind::Keyword_Enum:
                output.enumerations.emplace_back(parse_enum(curr())); break;
              default: UNREACHABLE;
            }
          }
          case Token_Kind::Colon_Equals: {
            auto name = curr();
            output.global_variables
              .emplace_back((const Syntax_Variable_Assignment*) parse_variable_assignment(name));
            break;
          }
          default: {
            this->diagnostics.build(Parse_Error {
              Parse_Error_Kind::Invalid_Token_After_Ident_In_Global_Scope,
              C1->location(),
              {Parse_Error::Metadata {C1}}
            });
            this->diagnostics.send_exception();
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



const Syntax_Node *
Parser::parse_variable_assignment(const Token *name)  {

  // Check 1: name: type = expr
  // Check 2: name := expr
  if (peek(1)->type() == Token_Kind::Colon) {
    auto E = new Syntax_Variable_Assignment(name, Some(parse_type()), parse_expr());
    expect<Token_Kind::Semi_Colon>("Expected ';' after declaration");
    return E;
  } else {
    auto E = new Syntax_Variable_Assignment(name, None, parse_expr());
    expect<Token_Kind::Semi_Colon>("Expected ';' after declaration");
    return E;
  }
}

/// Pratt Parsing Expression Technique (Precedence Matching)
/// References: https://en.wikipedia.org/wiki/Operator-precedence_parser
const Syntax_Node *
Parser::parse_expr(int previous_binding)  {
  Syntax_Node *lhs;

  auto first_token = curr();
  auto first_kind = first_token->type();

  // ~ : Bitwise NOT
  // + : Positive
  // - : Negation
  // & : allocator Reference
  // * : pointer Dereference
  // not : Logical Negation
  auto unary_binding = Parser::UnaryOperatorBinding(first_kind);
  if (unary_binding > previous_binding) {
    const auto expr  = parse_expr(unary_binding);
    const auto op = Syntax_Unary::MatchOp(first_kind);
    return new Syntax_Unary(op, first_token, expr);
  } else {
    lhs = (Syntax_Node*) match_expr();
  }
  next(1);
  do {
    auto op = curr();
    auto op_kind = op->type();

    const auto op_bindings = Parser::InfixOperatorBinding(op_kind);
    if (/* LeftBindingPrecedence */ op_bindings[0] <= previous_binding || op_bindings[0] == -1) {
      break;
    }
    skip(1);
    const auto rhs = parse_expr(/* RightBindingPrecedence */op_bindings[1]);
    auto new_expr = new Syntax_Binary {
      Syntax_Binary::MatchOp(op_kind), op, lhs, rhs
    };
    lhs = new_expr;
 } while (curr()->is_valid_expression_type());
  return lhs;
}

const NG_INLINE Syntax_Node *
Parser::match_expr() {
  auto matched_token = curr();
  switch (matched_token->type()) {

    // We have reached the EOF somehow, build an error for it here
    case EOFToken:
      this->diagnostics.build(Parse_Error {
        Parse_Error_Kind::Unexpected_End_Of_File,
        matched_token->location()
      });
      break;

    // Two Cases:
    // 1. A Variable  ... = my_ident_which_is_var
    // 2. A Function Call  ... nice_func(1, 2)
    case Identifier: {
      auto C1 = peek(1);
      if (C1->type() == Token_Kind::LParenthesis) {
        return parse_function_call(matched_token, C1);
      }
      else if (C1->type() == Token_Kind::LCurly_Brace) {
        // We are maybe in a struct instantiation
        auto cond1 = peek(2);
        auto cond2 = peek(3);
        if (cond1 && cond2 && cond1->type() == Token_Kind::Identifier &&
            cond2->type() == Token_Kind::Colon) {
          return parse_struct_instantiation(matched_token);
        }
      }
      else if (C1->type() == Token_Kind::Path) {
        // We are in a path value (ie: SomeEnum::Value)
        return parse_path(matched_token);
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
      skip(1);
      auto E = parse_expr();
      expect_delim<Token_Kind::RParenthesis>(curr()->location());
      position--;
      return E;
    }

    // List Initializer
    case LBracket: { // ex: [1, 2, 3]
      auto current = next(1);
      auto E  = new Syntax_List;

      // Loop through with reaching the EOF or closing expect_delim
      while (current->type() != Token_Kind::RBracket && current->type() != Token_Kind::EOFToken) {

        E->values.emplace_back(this->parse_expr());
        current = curr();

        if(current->type() == Token_Kind::Comma) skip(1);
        else if (current->type() == Token_Kind::RBracket) break;
        else expect<Token_Kind::Comma>("Expected ',' or ']' here");

      }
      this->expect_delim<Token_Kind::RBracket>(matched_token->location());
      position--;
      auto node = (Syntax_Node*)E;
      node->kind = Syntax_Kind::List;
      return node;
    }
    default: // TODO: Unexpected Expression Token
      break;
  }
  return nullptr;
}


Syntax_Type *Parser::parse_type()  {

  // []i32
  // **i32
  // *****i32
  // (i32, i32)

  auto first_token = curr();
  switch(first_token->type()) {
    case LBracket: {
      expect<Token_Kind::RBracket>("Sequence '[]' (representing array) must be"
                                  " met with closing value ']'");
      Syntax_Typename ty_name;
      auto ty_name_tok = curr();
      if (!ty_name_tok->is_valid_typename_start()) {
        auto ty = parse_type();
        return new Syntax_Type(Syntax_Type_Annotation::ArrayType, None, ty);
      }
      else if (!ty_name_tok->is_valid_typename()) {
        // TODO: Add type error value here
      }
      else {
        ty_name = Syntax_Type::MatchTypename(ty_name_tok);
      }
      return new Syntax_Type(Syntax_Type_Annotation::ArrayType, ty_name, nullptr);
    }
    case LParenthesis:
      // TODO: Add tuples
      break;
    case Star: {
      Syntax_Typename ty_name;
      auto ty_name_tok = next(1);
      if (!ty_name_tok->is_valid_typename_start()) {
        auto ty = parse_type();
        return new Syntax_Type(Syntax_Type_Annotation::Pointer, None, ty);
      }
      else if (!ty_name_tok->is_valid_typename()) {
        // TODO: Add type error value here
      }
      else {
        ty_name = Syntax_Type::MatchTypename(ty_name_tok);
      }
      return new Syntax_Type(Syntax_Type_Annotation::Pointer, ty_name, nullptr);
    }
    default:
      UNREACHABLE;
  }
  return new Syntax_Type(None, None, nullptr);
}


const Syntax_Node *
Parser::parse_for()  {
  auto loop_variable = skip<Token_Kind::Identifier,
                            Parse_Error_Kind::Missing_For_Loop_Variable>();
  skip<Token_Kind::Keyword_In, Parse_Error_Kind::Expected_Token>();
  auto list_or_range = skip(1);
  switch (list_or_range->type()) {
    case Identifier: {
      auto body = parse_block();
      return new Syntax_ForList(list_or_range, loop_variable, body);
    }
    case Keyword_Range: {
      expect<Token_Kind::LParenthesis>("");
      Range_Function func;
      FOR(value, 3) {
        if (curr()->type() == Token_Kind::RParenthesis || curr()->type() == Token_Kind::EOFToken)
          break;
        func.set[value] = expect<Token_Kind::Integer>("Range function values "
                                                      "must "
                                                      "be "
                                                      "integers")->get_value<size_t>();
        expect<Token_Kind::Comma>("Expected comma in between parameters.");
      }
      auto body = parse_block();
      return new Syntax_ForRange(loop_variable, func, body);
    }
    default:
      this->diagnostics.build(Parse_Error {
        Parse_Error_Kind::Invalid_Token,
        list_or_range->location(),
        {Parse_Error::Metadata("Invalid token in for-loop context. Expected "
                               "a list variable or the range function.") }
      });
      break;
  }
  return nullptr;
}

// TODO: Finish match statement. Leave this for later as it will be difficult
//  to see the outlook of this right now.
const NG_INLINE Syntax_Node *
Parser::parse_match()  {
  return nullptr;
}
const NG_INLINE Syntax_Node *
Parser::parse_match_pair_value()  {
  return nullptr;
}

std::vector<Syntax_Function_Parameter>
Parser::parse_function_param() {
  skip<Token_Kind::LParenthesis, Parse_Error_Kind::Expected_Token>();
  auto begin = (Syntax_Function_Parameter*) mem::GLOBAL_OBJECT_ALLOC.current();
  auto _ = (Syntax_Function_Parameter*) nullptr;

  while (curr()->type() not_eq Token_Kind::RParenthesis
          and
         curr()->type() not_eq Token_Kind::EOFToken) {
    auto param_name = skip<Token_Kind::Identifier,
                                    Parse_Error_Kind::Expected_Identifier_For_Function_Parameter>();
    expect<Token_Kind::Colon>("Expected ':' before parameter type (this is not Go!)");
    _ = new Syntax_Function_Parameter(param_name, parse_type());
  }
  expect_delim<Token_Kind::RParenthesis>(curr()->location());
  return std::vector<Syntax_Function_Parameter>(begin, (Syntax_Function_Parameter*) new char('a'));
}

const Syntax_Function *
Parser::parse_function(const Token *function_name) {
  auto type   = this->parse_type();
  auto block  = this->parse_block();
  auto params = this->parse_function_param();
  return new Syntax_Function(function_name, type, block, params);
}

const NG_INLINE Syntax_Node *
Parser::parse_function_call(const Token *function_name, const Token *delim) {
  auto E = new Syntax_Function_Call(function_name);
  while(curr()->type() != Token_Kind::RParenthesis && curr()->type() != Token_Kind::EOFToken) {
    E->parameters.emplace_back(parse_expr());
  }
  expect_delim<Token_Kind::RParenthesis>(delim->location());
  return E;
}

Syntax_Block Parser::parse_block(bool ret)  {
  expect<Token_Kind::LCurly_Brace>("Expected '{' before beginning of "
                                       "current");
  auto block   = Syntax_Block {};

  // We know for sure that we got right curly brace
  while (curr()->type() != Token_Kind::RCurly_Brace && curr()->type() != Token_Kind::EOFToken) {
    auto stmt = parse_stmt();
    block.statements.emplace_back(stmt);
    if (ret) {
      if (stmt->kind == Syntax_Kind::Return_Value)
        block.returns.emplace_back(stmt);
    }
  }
  expect_delim<Token_Kind::RCurly_Brace>(curr()->location());
  next(1);
  return block;
}

std::vector<Syntax_Struct_Member>
Parser::parse_struct_data_members() {
  auto begin = (Syntax_Struct_Member*) mem::GLOBAL_OBJECT_ALLOC.current();
  auto _ = (Syntax_Struct_Member*) nullptr;

  while (curr()->type() not_eq Token_Kind::RCurly_Brace) {
    auto name = expect<Token_Kind::Identifier>("Identifiers must be used for struct name");
    expect<Token_Kind::Colon>("':' Must be followed after struct member name");
    _ = new Syntax_Struct_Member(name, parse_type());
  }
  return std::vector<Syntax_Struct_Member>(begin, (Syntax_Struct_Member*) new char('a'));
}

const Syntax_Node *
Parser::parse_path(const Token *type) {
  auto value = expect<Token_Kind::Identifier>("Path value must be an "
                                             "identifier.");
  if (curr()->type() == Token_Kind::LParenthesis) {
    // Path Struct Value
  }
  return new Syntax_Path(type, value);
  //F0C452
}

const Syntax_Node *
Parser::parse_struct_instantiation(const Token *name)  {
  auto begin = (Syntax_Struct_Member_Initialization*) (mem::GLOBAL_OBJECT_ALLOC.current());
  auto _ = (Syntax_Struct_Member_Initialization*) nullptr;
  while (curr()->type() not_eq Token_Kind::RCurly_Brace
         and
         curr()->type() not_eq Token_Kind::EOFToken) {
    auto struct_name = expect<Token_Kind::Identifier>("Identifiers must be used for struct "
                                                      "member name");
    expect<Token_Kind::Colon>("':' Must be followed after struct member name");
    auto value = parse_expr();
    _ = new Syntax_Struct_Member_Initialization(struct_name, value);
  }
  expect_delim<Token_Kind::RCurly_Brace>(curr()->location());
  auto values = std::vector<Syntax_Struct_Member_Initialization> {
    begin, (Syntax_Struct_Member_Initialization *) new char('a')
  };
  return new Syntax_Struct_Instantiation(name,values);
}

const Syntax_Enum *
Parser::parse_enum(const Token *name) {
  auto enumeration = new Syntax_Enum;
  enumeration->name = name;
  while (curr()->type() != Token_Kind::RCurly_Brace && curr()->type() != Token_Kind::EOFToken) {
    auto constant = expect<Token_Kind::Identifier>("Identifiers must be used for enum value");
    if (curr()->type() == Token_Kind::Comma) {
      skip(1);
    }
    else if (curr()->type() == Token_Kind::LCurly_Brace) {
      skip(2);
      enumeration->enumeration_values.emplace_back(Syntax_Enum::Value(parse_struct(constant)));
    }
    else {
      if (curr()->type() != Token_Kind::RCurly_Brace) {
        // TODO: Add Error Here
      } else {
        enumeration->enumeration_values.emplace_back(Syntax_Enum::Value(constant));
      }
    }
  }
  expect_delim<Token_Kind::RCurly_Brace>(curr()->location());
  return enumeration;
}
