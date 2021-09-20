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

void Parser::parse() {
  for (;;) {
    if (fatal > 2) {
      this->diagnostics.send_exception();
    }
    switch (curr()->getKind()) {
      case TokenKind::EOFToken:
        return;
      case TokenKind::KeywordIf:
        skip(1);
        parse_if();
        break;
      case TokenKind::KeywordFor:
        skip(1);
        parse_for();
        break;
      case TokenKind::KeywordMatch:
        skip(1);
        parse_match();
        break;
      case TokenKind::Identifier: {
        auto C1 = peek(1);
        auto C2 = peek(2);

        if (C1->getKind() == TokenKind::FunctionArrow) {
          switch (C2->getKind()) {
            case TokenKind::LParenthesis: // Function
              parse_function(curr());
              break;
            case TokenKind::KeywordStruct: // Struct
              parse_struct(curr());
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
          parse_variable_assignment(name);
        }
        break;
      }
      default: // TODO: Figure out what to do here
        break;
    }
  }
}

Token *Parser::peek(size_t n)  {
  return tokens[position + n];
}

Token *Parser::skip(size_t n)  {
  auto *ret = tokens[position];
  position += n;
  return ret;
}

Token *Parser::next(size_t n) {
  position += n;
  return tokens[position];
}

template<TokenKind TK, ParseErrorType PE>
Token *Parser::skip() {
  auto next = tokens[(position++)];
  if (next->getKind() != TK) {
    this->diagnostics.build(ParseError(
      PE,
      next->getSourceLocation()
    ));
  }
  return next;
}

template<TokenKind TK, size_t N>
Token *Parser::expect(char const (&msg)[N]) {
  Token *next = tokens[(position++)];
  if (next->getKind() != TK) {
    this->diagnostics.build(ParseError(
      ParseErrorType::ExpectedToken, 
      next->getSourceLocation(),
      {
        ParseError::Metadata { TK, next, msg }
      }
    ));
    fatal++;
  }
  return next;
}

template<TokenKind TK>
void Parser::expect_delim(TokenTraits::SourceLocation const &loc) {
  Token *next = tokens[position++];
  if (next->getKind() != TK) {
    this->diagnostics.build(ParseError (
      ParseErrorType::MissingClosingPair,
      next->getSourceLocation(),
      { ParseError::Metadata { TK }, ParseError::Metadata { loc } }
    ));
  }
}

SyntaxNode *Parser::parse_variable_assignment(Token *name)  {
  SyntaxVariableAssignment *E;

  auto expr = parse_expr();
  expect<TokenKind::SemiColon>("Expected ';' after declaration");

  // Check 1: var name: type = expr
  // Check 2: var name = expr
  if (peek(1)->getKind() == TokenKind::Colon) {
    E = new SyntaxVariableAssignment(name, Some(parse_type()), expr);
    goto ret;
  } else {
    E = new SyntaxVariableAssignment(name, None, expr);
    goto ret;
  }
ret:
  auto node = static_cast<SyntaxNode*>(E);
  node->kind = SyntaxKind::VariableAssignment;
  return node;
}

SyntaxNode *Parser::parse_decl() {
  switch (curr()->getKind()) {
    case TokenKind::KeywordVar:
      skip(1);
      break;
    default:
      break;
  }
  return nullptr;
}


/// Pratt Parsing Expression Technique (Precedence Matching)
/// References: https://en.wikipedia.org/wiki/Operator-precedence_parser
SyntaxNode *Parser::parse_expr(int previous_binding)  {
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
    lhs = match_expr();
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

SyntaxNode *Parser::match_expr() {
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
      auto E = parse_expr();
      expect_delim<TokenKind::RParenthesis>(matched_token->getSourceLocation());
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
      auto node = (SyntaxNode*)E;
      node->kind = SyntaxKind::List;
      return node;
    }
    default: // TODO: Unexpected Expression Token
      break;
  }
  return nullptr;
}


SyntaxNode *Parser::parse_stmt()  {
  switch (curr()->getKind()) {
    case KeywordIf:
      break;
    case KeywordWhile:
      break;
    case KeywordFor:
      break;
    case KeywordBreak:
      break;
    case KeywordContinue:
      break;
    case KeywordDefer:
      break;
    case KeywordNone:
      break;
    case KeywordReturn:
      break;
    case KeywordMatch:
      break;
    case Identifier: {
      auto C1 = peek(1);
      if (C1->getKind() == TokenKind::ColonEquals) {
        auto name = curr();
        skip(2);
        return parse_variable_assignment(name);
      }
      break;
    }
    case LCurlyBrace:
      break;
    case Then:
      break;
    case At:
      break;
    case Error:
      break;
    case EOFToken:
      break;
    default: break;
  }
  return nullptr;
}

SyntaxType Parser::parse_type()  {
  return SyntaxType(None, None, None);
}


SyntaxNode *Parser::parse_if()  {
  auto E = new SyntaxIf;

  E->condition = parse_expr();
  E->body = parse_block();

  auto is_else = next(1);
  if (is_else->getKind() == TokenKind::KeywordElse) {
    E->else_ = new SyntaxElse(parse_block());
  }
  else if (is_else->getKind() == TokenKind::KeywordElif) {
    E->elif = parse_if();
    E->elif->kind = SyntaxKind::Elif;
  }

  auto node = (SyntaxNode*) E;
  node->kind = SyntaxKind::If;
  return node;
}

SyntaxNode *Parser::parse_while() {
  auto E = new SyntaxWhile;

  // Yes ... It really is this easy to parse a while loop.
  E->condition = parse_expr();

  auto v = curr();
  ASSERT_EQ(v->getKind(), TokenKind::LCurlyBrace);
  E->body = parse_block();

  auto ret = (SyntaxNode*) E;
  ret->kind = SyntaxKind::While;
  return ret;
}


SyntaxNode *Parser::parse_for()  {
  auto loop_variable = skip<TokenKind::Identifier,
                            ParseErrorType::MissingForLoopVariable>();
  skip<TokenKind::KeywordIn, ParseErrorType::ExpectedToken>();
  auto Begin = skip(1);
  return nullptr;
}

SyntaxNode *Parser::parse_match()  {
  return nullptr;
}

SyntaxNode *Parser::parse_match_pair_value()  {
  return nullptr;
}

void Parser::parse_function_param(SyntaxFunction *fn) {
  skip<TokenKind::LParenthesis, ParseErrorType::ExpectedToken>();
  auto current = curr();

  fn->parameters.begin = (SyntaxFunctionParameter*)(GLOBAL_OBJECT_ALLOC.current());
  while (current->getKind() != TokenKind::RParenthesis) {
    auto param_name = skip<TokenKind::Identifier,
                     ParseErrorType::ExpectedIdentifierForFunctionParameter>();
    expect<TokenKind::Colon>("Expected ':' before parameter type (this is not"
                             " go!)");

    fn->parameters.end = new SyntaxFunctionParameter(param_name, parse_type());
  }
}

SyntaxNode *Parser::parse_function(Token *function_name) {
  auto E = new SyntaxFunction;
  E->function_name = function_name;
  parse_function_param(E);

  E->function_type   = parse_type();
  E->body = parse_block();
  E->kind = SyntaxKind::FunctionDefault;
  return E;
}

SyntaxNode *Parser::parse_function_call(Token *function_name, Token *delim) {
  auto E = new SyntaxFunctionCall;
  E->function_name = function_name;
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
  auto open = expect<TokenKind::LCurlyBrace>("Expected '{' before beginning of "
                                       "current");
  auto block   = SyntaxBlock {};
  while (curr()->getKind() != TokenKind::RCurlyBrace) {
    block.statements << parse_stmt();
  }
  expect_delim<TokenKind::RParenthesis>(open->getSourceLocation());
  return block;
}

SyntaxNode *Parser::parse_struct(Token *struct_name)  {
  return nullptr;
}

SyntaxNode *Parser::parse_struct_function(Token *s, Token *func_name) {
  return nullptr;
}
