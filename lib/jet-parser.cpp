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
              break;
            case TokenKind::KeywordStruct: // Struct
              break;
            case TokenKind::KeywordEnum: // Enum
              break;
            default:
              UNREACHABLE;
              break;
          }
        }
        break;
      }
      default: // TODO: Figure out what to do here
        break;
    }
  }
}

Token *Parser::peek(size_t n)  {
  return tokens + position + n;
}

Token *Parser::skip(size_t n)  {
  auto *ret = tokens + position;
  position += n;
  return ret;
}

Token *Parser::next(size_t n) {
  position += n;
  return tokens + position;
}

template<TokenKind TK, ParseErrorType PE>
Token *Parser::skip() {
  auto next = tokens + (position++);
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
  auto next = tokens + (position++);
  if (next->getKind() != TK) {
    this->diagnostics.build(ParseError(
      ParseErrorType::ExpectedToken, 
      next->getSourceLocation(),
      {
        ParseError::Metadata { TK, next->getKind(), msg }
      }
    ));
    fatal++;
  }
  return next;
}

template<TokenKind TK>
void Parser::delim(const TokenTraits::SourceLocation &loc) {
  Token next = tokens[position++];
  if (next.getKind() != TK) {
    this->diagnostics.build(ParseError (
      ParseErrorType::MissingClosingPair,
      next.getSourceLocation(),
      { ParseError::Metadata { TK }, ParseError::Metadata { loc } }
    ));
  }
}

SyntaxNode *Parser::parse_variable_decl()  {
  auto E = arena::allocate<SyntaxVariableAssignment>(1);
  auto name = skip(1);

  if (name->isKeyword()) {
    this->diagnostics.build(ParseError{
      ParseErrorType::ReservedIdentifierAsVariableName,
      name->getSourceLocation()
    });
    fatal++;
  }
  else if (name->getKind() != TokenKind::Identifier) {
    this->diagnostics.build(ParseError{
      ParseErrorType::MissingVariableName,
      name->getSourceLocation()
    });
    fatal++;
  }

  expect<TokenKind::Equals>("Missing '=' before variable "
                                       "expression");
  auto expr = parse_expr();

  // Check 1: var name: type = expr
  // Check 2: var name = expr
  if (peek(1)->getKind() == TokenKind::Colon) {
    E->name = name;
    E->type = Some(parse_type());
    E->expression = expr;
    goto ret;
  } else {
    E->name = name;
    E->type = None;
    E->expression = expr;
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
      return parse_variable_decl();
    default:
      break;
  }
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

    const auto E = arena::allocate<SyntaxUnary>();
    E->kind = SyntaxKind::Unary;
    *E = {
      op, first_token, expr
    };
    return expr;
  } else {
    lhs = match_expr();
  }

  while (curr()->getKind() != EOFToken) {
    auto op = next(1);
    auto op_kind = op->getKind();

    const auto op_bindings = Parser::InfixOperatorBinding(op_kind);
    if (/* LeftBindingPrecedence */ op_bindings[0] <= previous_binding)
      break;
    skip(1);
    const auto rhs = parse_expr(/* RightBindingPrecedence */
      op_bindings[1]);
    auto new_expr = arena::allocate<SyntaxBinary>(1);
    *new_expr = SyntaxBinary {
      SyntaxBinary::MatchOp(op_kind), op, lhs, rhs
    };
    lhs = new_expr;
    lhs->kind = SyntaxKind::Binary;
  }
  return lhs;
}

SyntaxNode *Parser::match_expr() {
  auto first_token = curr();
  switch (first_token->getKind()) {
    case EOFToken:
      this->diagnostics.build(ParseError {
        ParseErrorType::UnexpectedEndOfFile,
        first_token->getSourceLocation()
      });
      break;
    case Identifier: {// Unresolved variable or Function Call
      if (peek(1)->getKind() == TokenKind::LParenthesis) {
        return parse_function_call();
      }
      auto E = arena::allocate<SyntaxLiteral>(1);
      *E = SyntaxLiteral { first_token };
      E->kind = SyntaxKind::LiteralValue;
      return E;
    }
    case Integer: case String: case Char: case Decimal: case KeywordTrue:
    case KeywordFalse: {
      auto E = arena::allocate<SyntaxLiteral>(1);
      E->literal = first_token;
      auto ret = static_cast<SyntaxNode*>(E);
      ret->kind = SyntaxKind::LiteralValue;
      return ret;
    }
    case LParenthesis: {
      auto E = parse_expr();
      delim<TokenKind::RParenthesis>(first_token->getSourceLocation());
      return E;
    }
    case LBracket: { // ex: [1, 2, 3]
      auto current = next(1);
      auto E  = arena::allocate<SyntaxList>();
      while (current->getKind() != TokenKind::RBracket && current->getKind()
      != TokenKind::EOFToken) {
        if (current->getKind() == TokenKind::Comma) {
          skip(1);
        }
        auto expr = parse_expr();
        vec::push(E->values, expr);
        current = curr();
      }
      delim<TokenKind::RBracket>(first_token->getSourceLocation());
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
    case KeywordVar:
      skip(1);
      return parse_variable_decl();
    case KeywordNone:
      break;
    case KeywordReturn:
      break;
    case KeywordMatch:
      break;
    case Identifier:
      break;
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
  auto E = arena::allocate<SyntaxIf>(1);
  E->condition = parse_expr();
  E->body = parse_block();


  auto node1 = E->body.statements[0];
  auto node2 = ((SyntaxVariableAssignment*)(node1))->expression;
  ASSERT(node1->kind == SyntaxKind::VariableAssignment, "NOO");
  ASSERT(node2->kind == SyntaxKind::LiteralValue,"NOO");
  auto is_else = next(1);
  if (is_else->getKind() == TokenKind::KeywordElse) {
    E->else_ = arena::allocate<SyntaxElse>();
    E->else_->body = parse_block();
  }
  else if (is_else->getKind() == TokenKind::KeywordElif) {
    E->elif = parse_if();
    E->elif->kind = SyntaxKind::Elif;
  }

  auto node = (SyntaxNode*) E;
  node->kind = SyntaxKind::If;
  return node;
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

void Parser::parse_function_param(SyntaxFunction *fn) {
  skip<TokenKind::LParenthesis, ParseErrorType::ExpectedToken>();
  auto current = curr();
  while (current->getKind() != TokenKind::RParenthesis) {
    auto param_name = skip<TokenKind::Identifier,
                     ParseErrorType::ExpectedIdentifierForFunctionParameter>();
    expect<TokenKind::Colon>("Expected ':' before parameter type (this is not"
                             " go!)");
    vec::push(fn->parameters, SyntaxFunctionParameter {
      param_name, parse_type()
    });
  }
}

SyntaxNode *Parser::parse_function() {
  auto E = arena::allocate<SyntaxFunction>(1);
  E->function_name = skip<TokenKind::Identifier,
                            ParseErrorType::MissingFunctionName>();
  parse_function_param(E);

  E->function_type   = parse_type();
  E->body = parse_block();
  E->kind = SyntaxKind::FunctionDefault;
  return E;
}

SyntaxNode *Parser::parse_function_call() {
  auto E = arena::allocate<SyntaxFunctionCall>(1);
  E->function_name = skip(1); // Guaranteed to be IDENT
  auto open = skip<TokenKind::LParenthesis,
  ParseErrorType::ExpectedToken>();
  while(curr()->getKind() != TokenKind::RParenthesis) {
    vec::push(E->parameters, parse_expr());
  }
  delim<TokenKind::RParenthesis>(open->getSourceLocation());
  return E;
}

SyntaxBlock Parser::parse_block()  {
  auto open = expect<TokenKind::LCurlyBrace>("Expected '{' before beginning of "
                                       "current");
  auto block   = SyntaxBlock {};

  auto current = curr();
  while (current->getKind() != TokenKind::RCurlyBrace) {
    vec::push(block.statements, parse_stmt());
    current = curr();
  }

  delim<TokenKind::RParenthesis>(open->getSourceLocation());
  return block;
}

SyntaxNode *Parser::parse_struct()  {
  return nullptr;
}

SyntaxNode *Parser::parse_struct_function(Token *s, Token *func_name) {
  return nullptr;
}