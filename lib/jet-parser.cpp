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

nextgen::ObjectVector<const SyntaxNode*, 20000>
Parser::parse() {

  // Mark space for at least 20000 nodes which will
  // likely be good for most programs.
  auto nodes = mem::ObjectVector<const SyntaxNode*, 20000>();

  for (;;) {

    // Report up to 2 errors before exiting the program
    if (fatal > 2) {
      this->diagnostics.send_exception();
    }

    switch (curr()->getKind()) {
      case TokenKind::EOFToken:
        return nodes;
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

const Token *Parser::peek(size_t n)  {
  return tokens[position + n];
}

const Token *Parser::skip(size_t n)  {
  auto *ret = tokens[position];
  position += n;
  return ret;
}

const Token *Parser::next(size_t n) {
  position += n;
  return tokens[position];
}

template<TokenKind TK, ParseErrorType PE>
const Token *Parser::skip() {
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
const Token *Parser::expect(char const (&msg)[N]) {
  const Token *next = tokens[(position++)];
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
      {
                  ParseError::Metadata { TK },
                  ParseError::Metadata { next },
                  ParseError::Metadata { loc }
                }
    ));
    this->diagnostics.send_exception();
  }
}

const NG_INLINE SyntaxNode *
Parser::parse_variable_assignment(const Token *name)  {

  // Check 1: name: type = expr
  // Check 2: name := expr
  if (peek(1)->getKind() == TokenKind::Colon) {
    auto E = new SyntaxVariableAssignment(name, Some(parse_type()), parse_expr());
    expect<TokenKind::SemiColon>("Expected ';' after declaration");
    return E;
  } else {
    auto E = new SyntaxVariableAssignment(name, None, parse_expr());
    expect<TokenKind::SemiColon>("Expected ';' after declaration");
    return E;
  }
}

/// Pratt Parsing Expression Technique (Precedence Matching)
/// References: https://en.wikipedia.org/wiki/Operator-precedence_parser
const NG_INLINE SyntaxNode *
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

const NG_INLINE SyntaxNode *
Parser::parse_stmt()  {
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


template<bool ELIF>
const NG_INLINE SyntaxNode *
Parser::parse_if()  {
  auto cond = parse_expr();
  auto body = parse_block();

  SyntaxElse *else_ = nullptr; SyntaxElif *elif = nullptr;
  auto is_else = next(1);
  if (is_else->getKind() == TokenKind::KeywordElse) {
    else_ = new SyntaxElse(parse_block());
  }
  else if (is_else->getKind() == TokenKind::KeywordElif) {
    elif = (SyntaxElif*)((SyntaxNode*) parse_if<true>());
  }
  auto E = (SyntaxNode*) new SyntaxIf(cond, body, else_, elif);
  if (ELIF)
    E->kind = SyntaxKind::Elif;
  return E;
}


const  SyntaxNode *
Parser::parse_while() {
  auto cond = parse_expr();
  auto body = parse_block();
  return new SyntaxWhile(cond, body);
}

const NG_INLINE SyntaxNode *
Parser::parse_for()  {
  auto loop_variable = skip<TokenKind::Identifier,
                            ParseErrorType::MissingForLoopVariable>();
  skip<TokenKind::KeywordIn, ParseErrorType::ExpectedToken>();
  auto Begin = skip(1);
  return nullptr;
}

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

const NG_INLINE SyntaxNode *
Parser::parse_struct(const Token *struct_name)  {
  return nullptr;
}

const NG_INLINE SyntaxNode *
Parser::parse_struct_function(const Token *s,
                                          const Token *func_name) {
  return nullptr;
}
