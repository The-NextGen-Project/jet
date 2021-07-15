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
// Pilot: ashishbailkeri123@gmail.com
// Parser.CPP
// Jet's Main Parser



#include <nextgen/jet/Parse/Parser.h>

using namespace nextgen::jet;

using nextgen::Vec;

void Parser::parse() {
  switch (curr()->getKind()) {
    case TokenKind::EOFToken:
      break;
    case TokenKind::KeywordVar:
      skip(1);
      parse_variable_decl();
      break;
    case TokenKind::KeywordStruct:
      skip(1);
      parse_struct();
      break;
    case TokenKind::KeywordFunction:
      skip(1);
      parse_function();
      break;
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
      auto C3 = peek(3);

      // TODO: What if we are in generics?
      if (C1->getKind() == TokenKind::Dot) {
        if (C2->getKind() == TokenKind::Identifier) {
          if (C3->getKind() == TokenKind::Equals) {
            // Ok, we are in special struct function
            skip(3);
            parse_struct_function(curr(), C2);
          }
        } else parse_expr();
      } else parse_expr();
      break;
    }
    default: // TODO: Figure out what to do here
      break;
  }
}

Token *Parser::peek(size_t n)  {
  return tokens.get_pointer_at(position + n);
}

Token *Parser::skip(TokenKind kind, ParseErrorType error_kind) {
  auto next = tokens.get_pointer_at(position++);
  if (next->getKind() != kind) {
    // TODO: Add Error Handling Here
    //PANIC("OH NO");

    return nullptr;
  }
  return next;
}

Token *Parser::skip(size_t n)  {
  auto *ret = tokens.get_pointer_at(position);
  position += n;
  return ret;
}

Token *Parser::next(size_t n) {
  skip(n);
  return curr();
}

int Parser::expect(TokenKind kind) {
  Token next = tokens[position++];
  if (next.getKind() != kind) {
    this->diagnostics.build(ParseError(
      ParseErrorType::ExpectedToken, 
      next.getSourceLocation(),
      {
        ParseError::Metadata { kind, next.getKind() }
      }
    ));
    return 0;
  }
  return 1;
}

SyntaxNode *Parser::parse_variable_decl()  {
  auto name = skip(1);

  if (name->isKeyword()) {
    this->diagnostics.build(ParseError{
      ParseErrorType::ReservedIdentifierAsVariableName,
      name->getSourceLocation()
    });
  }

  if (name->getKind() != TokenKind::Identifier) {
    this->diagnostics.build(ParseError{
      ParseErrorType::MissingVariableName,
      name->getSourceLocation()
    });
  }

  auto res = expect(TokenKind::Equals);
  if (!res) { this->fatal = true; return nullptr; }

  auto expr = parse_expr();

  // var my_var = 23
  // var my_var: int = 23

  auto E = allocator->allocate<SyntaxNode>();
  E->kind = SyntaxKind::VariableAssignment;

  if (peek(1)->getKind() == TokenKind::Colon) {
    auto type = parse_type();
    E->VariableAssignment = {
      name, type, expr
    };
    return E;
  } else {
    E->VariableAssignment = SyntaxVariableAssignment {
      name, None, expr
    };
    return E;
  }
}

auto Parser::parse_decl() -> SyntaxNode* {
  return nullptr;
}


/// Pratt Parsing Expression Technique (Precedence Matching)
/// References: https://en.wikipedia.org/wiki/Operator-precedence_parser
SyntaxNode *Parser::parse_expr(int previous_binding)  {
  auto lhs = allocator->allocate<SyntaxNode>(1);
  auto first_token = curr();
  auto first_kind = first_token->getKind();

  // ~ : Bitwise NOT
  // + : Positive
  // - : Negation
  // & : allocator Reference
  // * : pointer Dereference
  auto unary_binding = Parser::UnaryOperatorBinding(first_kind);
  if (unary_binding > previous_binding) {
    auto E = parse_expr(unary_binding);
    const auto op = SyntaxUnary::MatchOp(first_kind);
    lhs->Unary = {
      op, first_token, E
    };
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
    auto new_expr = allocator->allocate<SyntaxNode>(1);
    new_expr->Binary = {
      SyntaxBinary::MatchOp(op_kind), lhs, rhs, op
    };
    new_expr->kind = SyntaxKind::Binary;
    // Move pointer to lhs
    lhs = new_expr;
  }
  return lhs;
}

SyntaxNode *Parser::match_expr() {
  auto first_token = curr();
  switch (first_token->getKind()) {
    case EOFToken:
      // TODO: Handle Error Case
      break;
    case Identifier: {// Unresolved variable or Function Call
      if (peek(1)->getKind() == TokenKind::LParenthesis) {
        // This is a function call
        // auto E = ParseFunctionCallExpression()
        // return E;
      }
    }
    case Integer:
    case String:
    case Char:
    case Decimal:
    case KeywordTrue:
    case KeywordFalse: {
      auto E = allocator->allocate<SyntaxNode>(1);
      E->Literal = {
        first_token
      };
      E->kind = SyntaxKind::LiteralValue;
      return E;
    }
    case LParenthesis: {
      auto E = parse_expr();
      skip(TokenKind::RParenthesis, ParseErrorType::MissingClosingParenthesis);
      return E;
    }
    case LBracket: { // ie: [1, 2, 3]
      auto current = next(1);
      auto values = Vec<SyntaxNode*>(5);
      while (current->getKind() != TokenKind::RBracket) {
        if (current->getKind() == TokenKind::EOFToken) {
          // TODO: Add Error
        }
        values.push(parse_expr());
        current = skip(1);
      }
      auto E  = allocator->allocate<SyntaxNode>();
      E->kind = SyntaxKind::List;
      E->List = SyntaxList { values };
      return E;
    }
    default: // TODO: Unexpected Expression Token
      break;
  }
  return nullptr;
}


SyntaxNode *Parser::parse_stmt()  {
  return nullptr;
}

Option<SyntaxType> Parser::parse_type()  {
  if (peek(1)->getKind() == TokenKind::Comma)
    return None;
  return None;
}

SyntaxNode *Parser::parse_if()  {
  auto cond = parse_expr();

  SyntaxBlock body;
  SyntaxNode *else_ = nullptr;
  SyntaxNode *elif_ = nullptr;

  body = parse_block();
  auto is_else = skip(1);
  if (is_else->getKind() == TokenKind::KeywordElse) {
    SyntaxBlock else_body = parse_block();
    else_ = allocator->allocate<SyntaxNode>(1);
    else_->kind = SyntaxKind::Else;
    else_->Else = {
      else_body
    };
  }
  else if (is_else->getKind() == TokenKind::KeywordElif) {
    elif_ = parse_if();
    elif_->kind = SyntaxKind::Elif;
  }

  auto E = allocator->allocate<SyntaxNode>(1);
  E->kind = SyntaxKind::If;
  E->If = {
    cond, body, else_, elif_
  };
  return E;
}

SyntaxNode *Parser::parse_for()  {
  auto LoopVar = skip(TokenKind::Identifier,
                      ParseErrorType::MissingForLoopVariable);
  skip(TokenKind::KeywordIn, ParseErrorType::ExpectedToken);

  auto Begin = skip(1);
  return nullptr;
}

auto Parser::parse_match() -> SyntaxNode * {
  return nullptr;
}

Vec<SyntaxFunctionParameter> Parser::parse_function_param() {
  skip(TokenKind::LParenthesis, ParseErrorType::ExpectedToken);
  auto current = curr();
  auto params  = Vec <SyntaxFunctionParameter> {};
  while (current->getKind() != TokenKind::RParenthesis) {
    auto param_name = skip(TokenKind::Identifier,
                           ParseErrorType::ExpectedIdentifierForFunctionParameter);
    expect(TokenKind::Colon);
    params.push({param_name, parse_type()});
  }
  return params;
}

SyntaxNode *Parser::parse_function() {
  auto E = allocator->allocate<SyntaxNode>(1);
  auto function_name = skip(TokenKind::Identifier,
                            ParseErrorType::MissingFunctionName);

  auto params        = parse_function_param();
  auto function_ty   = parse_type();
  auto function_body = parse_block();

  E->kind = SyntaxKind::FunctionDefault;
  E->Function = {
    function_name, function_ty, function_body, params
  };
  return E;
}

SyntaxNode *Parser::parse_function_call() {
  auto function_name = skip(1); // Guaranteed to be IDENT
  auto params = Vec<SyntaxNode*>{};
  expect(TokenKind::LParenthesis);
  while(curr()->getKind() != TokenKind::RParenthesis) {
    params.push(parse_expr());
  }
  expect(TokenKind::RParenthesis);
  auto E = allocator->allocate<SyntaxNode>(1);
  E->kind = SyntaxKind::FunctionCall;
  E->FunctionCall = {
    function_name, params
  };
  return E;
}

SyntaxBlock Parser::parse_block()  {
  expect(TokenKind::LCurlyBrace);
  auto current = curr();
  auto statements = Vec<SyntaxNode*>{};
  while (current->getKind() != TokenKind::RCurlyBrace) {
    statements.push(parse_stmt());
  }
  expect(TokenKind::RCurlyBrace);
  return {
    statements
  };
}

SyntaxNode *Parser::parse_struct()  {
  return nullptr;
}

SyntaxNode *Parser::parse_struct_function(Token *s, Token *func_name) {
  return nullptr;
}