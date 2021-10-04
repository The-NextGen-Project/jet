//                                              ////
//                                               ////.
//                                               */////
//                                                ///////
//                                      ////       ///////
//                                       //////////////(((((((((((((((
//                                      /////******((((((((/////////,
//                                                *((((((
//                                                (((((/
//                                               (((((
//                                              ((((.
//
//
//
//                              ,(((((,    ((((((((((((#####   ####################
//                              (((((((   *((((((((########## ####################%%
//                              (((((((   *(((((###########     ##############%%%%
//                              (((((((   *(#####///////*             ####%%,
//                              (((((((   *################           %%%%%%,
//                              ((((###   *################           %%%%%%,
//                    (((((     #######   /######                     %%%%%%,
//                   (((((((##########.   /################%          %%%%%%,
//                    *(#############     ,############%%%%%%         %%%%%%,
//                        /######,          #######%%%%%%%%*           %%%%
//
//
//
// Offered under the MIT License.
// Authors:
//    • Ashish Bailkeri <ashishbailkeri123@gmail.com>
//



# include <nextgen/jet/jet-parser.h>

using namespace nextgen::jet;

const ParserOutput
Parser::parse() {
  auto output = ParserOutput();

  for (;;) {

    // Report up to 2 errors before exiting the program
    if (fatal > 2) {
      this->diagnostics.send_exception();
    }

    switch (curr()->getKind()) {
      case TokenKind::EOFToken:
        return output;
      case TokenKind::Identifier: {
        auto C1 = peek(1);
        auto C2 = peek(2);

        if (C1->getKind() == TokenKind::FunctionArrow) {
          switch (C2->getKind()) {
            case TokenKind::LParenthesis: // Function
              output.functions << parse_function(curr());
              break;
            case TokenKind::KeywordStruct: // Struct
              output.structures << parse_struct(curr());
              break;
            case TokenKind::KeywordEnum: // Enum
              break;
            default:
              UNREACHABLE;
              break;
          }
        }
        else if (C1->getKind() == TokenKind::ColonEquals) {
          auto name = curr();
          skip(2);
          output.global_variables <<
          (const SyntaxVariableAssignment*)
            parse_variable_assignment(name);
        }
        else {
          this->diagnostics.build(ParseError {
            ParseErrorType::InvalidTokenAfterIdentInGlobalScope,
            C1->getSourceLocation(),
            { ParseError::Metadata { C1 }}
          });
          this->diagnostics.send_exception();
        }
        break;
      }
      default: // TODO: Figure out what to do here
        break;
    }
  }
  return output;
}



const SyntaxNode *
Parser::parse_variable_assignment(const Token *name)  {

  // Check 1: name: type = expr
  // Check 2: name := expr
  if (peek(1)->getKind() == TokenKind::Colon) {
    auto E = new SyntaxVariableAssignment(name, Some(parse_type()), parse_expr());
    expect<TokenKind::SemiColon>("Expected ';' after declaration");
    --position;
    return E;
  } else {
    auto E = new SyntaxVariableAssignment(name, None, parse_expr());
    expect<TokenKind::SemiColon>("Expected ';' after declaration");
    --position;
    return E;
  }
}

/// Pratt Parsing Expression Technique (Precedence Matching)
/// References: https://en.wikipedia.org/wiki/Operator-precedence_parser
const SyntaxNode *
Parser::parse_expr(int previous_binding)  {
  SyntaxNode *lhs;

  auto first_token = curr();
  auto first_kind = first_token->getKind();

  // ~ : Bitwise NOT
  // + : Positive
  // - : Negation
  // & : allocator Reference
  // * : pointer Dereference
  // not : Logical Negation
  auto unary_binding = Parser::UnaryOperatorBinding(first_kind);
  if (unary_binding > previous_binding) {
    const auto expr  = parse_expr(unary_binding);
    const auto op = SyntaxUnary::MatchOp(first_kind);

    const auto E = (SyntaxNode*)(new SyntaxUnary(op, first_token, expr));
    E->kind = SyntaxKind::Unary;
    return expr;
  } else {
    lhs = (SyntaxNode*) match_expr();
  }

   do {
    auto op = next(1);
    auto op_kind = op->getKind();

    const auto op_bindings = Parser::InfixOperatorBinding(op_kind);
    if (/* LeftBindingPrecedence */ op_bindings[0] <= previous_binding) {
      //skip(1);
      break;
    }
    auto a = skip(1);
    const auto rhs = parse_expr(/* RightBindingPrecedence */
      op_bindings[1]);
    auto new_expr = new SyntaxBinary {
      SyntaxBinary::MatchOp(op_kind), op, lhs, rhs
    };
    lhs = new_expr;
    lhs->kind = SyntaxKind::Binary;
   } while (curr()->isValidExpressionType());
  return lhs;
}

const NG_INLINE SyntaxNode *
Parser::match_expr() {
  auto matched_token = curr();
  switch (matched_token->getKind()) {
    case EOFToken:
      this->diagnostics.build(ParseError {
        ParseErrorType::UnexpectedEndOfFile,
        matched_token->getSourceLocation()
      });
      break;
    case Identifier: { // Unresolved variable or Function Call
      auto C1 = peek(1);
      if (peek(1)->getKind() == TokenKind::LParenthesis) {
        return parse_function_call(matched_token, C1);
      }
      auto E = (SyntaxNode*) new SyntaxLiteral(matched_token);
      E->kind = SyntaxKind::LiteralValue;
      return E;
    }
    case Integer: case String: case Char: case Decimal: case KeywordTrue:
    case KeywordFalse: {
      auto E = (SyntaxNode*) new SyntaxLiteral(matched_token);
      E->kind = SyntaxKind::LiteralValue;
      return E;
    }
    case LParenthesis: {
      skip(1);
      auto E = parse_expr();
      expect_delim<TokenKind::RParenthesis>(matched_token->getSourceLocation());
      position--;
      return E;
    }
    case LBracket: { // ex: [1, 2, 3]
      auto current = next(1);
      auto E  = new SyntaxList;

      // Loop through with reaching the EOF or closing expect_delim
      while (current->getKind() != TokenKind::RBracket
            && current->getKind() != TokenKind::EOFToken) {

        // Skip separating expect_delim
        if (current->getKind() == TokenKind::Comma) {
          skip(1);
        }

        E->values << parse_expr();
        current = curr();
      }
      expect_delim<TokenKind::RBracket>(matched_token->getSourceLocation());
      position--;
      auto node = (SyntaxNode*)E;
      node->kind = SyntaxKind::List;
      return node;
    }
    default: // TODO: Unexpected Expression Token
      break;
  }
  return nullptr;
}


SyntaxType *Parser::parse_type()  {

  // []i32
  // **i32
  // *****i32
  // (i32, i32)

  auto first_token = curr();
  switch(first_token->getKind()) {
    case LBracket: {
      expect<TokenKind::RBracket>("Sequence '[]' (representing array) must be"
                                  " met with closing value ']'");
      SyntaxTypename ty_name;
      auto ty_name_tok = curr();
      if (! ty_name_tok->isValidTypenameStart()) {
        auto ty = parse_type();
        return new SyntaxType(SyntaxTypeAnnotation::ArrayType, None, ty);
      }
      else if (! ty_name_tok->isValidTypename()) {
        // TODO: Add type error value here
      }
      else {
        ty_name = SyntaxType::MatchTypename(ty_name_tok);
      }
      return new SyntaxType(SyntaxTypeAnnotation::ArrayType, ty_name, nullptr);
    }
    case LParenthesis:
      // TODO: Add tuples
      break;
    case Star: {
      SyntaxTypename ty_name;
      auto ty_name_tok = next(1);
      if (! ty_name_tok->isValidTypenameStart()) {
        auto ty = parse_type();
        return new SyntaxType(SyntaxTypeAnnotation::Pointer, None, ty);
      }
      else if (! ty_name_tok->isValidTypename()) {
        // TODO: Add type error value here
      }
      else {
        ty_name = SyntaxType::MatchTypename(ty_name_tok);
      }
      return new SyntaxType(SyntaxTypeAnnotation::Pointer, ty_name, nullptr);
    }
    default:
      UNREACHABLE;
  }
  return new SyntaxType(None, None, nullptr);
}


const SyntaxNode *
Parser::parse_for()  {
  auto loop_variable = skip<TokenKind::Identifier,
                            ParseErrorType::MissingForLoopVariable>();
  skip<TokenKind::KeywordIn, ParseErrorType::ExpectedToken>();
  auto list_or_range = skip(1);
  switch (list_or_range->getKind()) {
    case Identifier: {
      auto body = parse_block();
      return new SyntaxForList(list_or_range, loop_variable, body);
    }
    case KeywordRange: {
      expect<TokenKind::LParenthesis>("");
      RangeFunction func;
      FOR(value, 3) {
        if (curr()->getKind() == TokenKind::RParenthesis
        || curr()->getKind() == TokenKind::EOFToken)
          break;
        func.set[value] = expect<TokenKind::Integer>("Range function values "
                                                    "must "
                                                 "be "
                                       "integers")->getValue<size_t>();
        expect<TokenKind::Comma>("Expected comma in between parameters.");
      }
      auto body = parse_block();
      return new SyntaxForRange(loop_variable, func, body);
    }
    default:
      this->diagnostics.build(ParseError {
        ParseErrorType::InvalidToken,
        list_or_range->getSourceLocation(),
        { ParseError::Metadata("Invalid token in for-loop context. Expected "
                               "a list variable or the range function.") }
      });
      break;
  }
  return nullptr;
}

// TODO: Finish match statement. Leave this for later as it will be difficult
//  to see the outlook of this right now.
const NG_INLINE SyntaxNode *
Parser::parse_match()  {
  return nullptr;
}
const NG_INLINE SyntaxNode *
Parser::parse_match_pair_value()  {
  return nullptr;
}

nextgen::ArenaVec<SyntaxFunctionParameter> Parser::parse_function_param() {
  skip<TokenKind::LParenthesis, ParseErrorType::ExpectedToken>();
  auto current = curr();


  SyntaxFunctionParameter *begin = (SyntaxFunctionParameter*)(GLOBAL_OBJECT_ALLOC.current());
  SyntaxFunctionParameter *end = nullptr;

  while (current->getKind() != TokenKind::RParenthesis) {
    auto param_name = skip<TokenKind::Identifier,
                     ParseErrorType::ExpectedIdentifierForFunctionParameter>();
    expect<TokenKind::Colon>("Expected ':' before parameter type (this is not"
                             " go!)");

    end = new SyntaxFunctionParameter(param_name, parse_type());
  }
  return mem::ArenaVec<SyntaxFunctionParameter>{begin, end};
}

const NG_INLINE SyntaxFunction *
Parser::parse_function(const Token *function_name) {
  return new SyntaxFunction(function_name,
                              parse_type(),
                              parse_block(),
                              parse_function_param());
}

const NG_INLINE SyntaxNode *
Parser::parse_function_call(const Token *function_name, const Token *delim) {
  auto E = new SyntaxFunctionCall(function_name);
  while(curr()->getKind() != TokenKind::RParenthesis) {

    // Looks odd, right? What's happening here is that the arena will allocate
    // nodes in its continuous memory block and the function will write to that
    // memory, so we don't need to worry about pushing values to the arena.
    //
    // TODO: Refactor some parsing functions to be void because of the arena
    //  allocation.
    E->parameters << parse_expr();
  }
  expect_delim<TokenKind::RParenthesis>(delim->getSourceLocation());
  return E;
}

SyntaxBlock Parser::parse_block()  {
  expect<TokenKind::LCurlyBrace>("Expected '{' before beginning of "
                                       "current");
  auto block   = SyntaxBlock {};
  // We know for sure that we got right curly brace
  while (curr()->getKind() != TokenKind::RCurlyBrace) {
    block.statements << parse_stmt();
  }
  next(1);
  return block;
}

nextgen::ArenaVec<SyntaxStructMember>
Parser::parse_struct_data_members() {
  auto members = mem::ArenaVec<SyntaxStructMember>();
  members.begin = (SyntaxStructMember*) (GLOBAL_OBJECT_ALLOC.current());
  while (curr()->getKind() != TokenKind::RCurlyBrace) {
    auto name = expect<TokenKind::Identifier>("Identifiers must be used for "
                                              "struct name");
    expect<TokenKind::Colon>("':' Must be followed after struct member name");
    auto type = parse_type();
    members.end = new SyntaxStructMember(name, type);
  }
  return members;
}