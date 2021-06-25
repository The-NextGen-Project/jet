# include <nextgen/Jet/Parse/Parser.h>

using namespace nextgen::jet;

void Parser::Parse() {
  switch (Curr()->getKind()) {
    case TokenKind::EOFToken:
      break;
    case TokenKind::KeywordVar:
      ParseVariableDecl();
      break;
    case TokenKind::KeywordIf:
      break;
    case TokenKind::KeywordFor:
      break;
    default: // TODO: Figure out what to do here
      break;
  }
}

auto NG_AINLINE Parser::Peek(size_t NTok) -> Token* {
  return Tokens[Position + NTok];
}

auto Parser::Skip(TokenKind Kind, ParseErrorType ErrorKind) -> Token* {
  auto Next = Tokens[Position++];
  if (Next->getKind() != Kind) {
    // TODO: Add Error Handling Here
    //PANIC("OH NO");
  }
  return Next;
}

auto Parser::Skip(size_t NTok) -> Token*  {
  Position += NTok;
  return Tokens[Position];
}


auto Parser::ParseVariableDecl() -> SyntaxExpression* {
  auto Name = Skip(TokenKind::Identifier, ParseErrorType::MissingVariableName);

  if (Name->isKeyword()) {
    // TODO: Add Error Here
  }

  Skip(TokenKind::Equals, ParseErrorType::ExpectedToken);
  auto Expr = ParseExpression();

  // var my_var = 23
  // var my_var: int = 23

  auto E = Memory->Next<SyntaxExpression>(1);
  E->Kind = SyntaxKind::VariableAssignment;

  if (Peek(1)->getKind() == TokenKind::Colon) {
    auto Type = ParseValueType();
    E->VariableAssignment = {
      Name, Type, Expr
    };
    return E;
  } else {


    E->VariableAssignment = {
      Name, SyntaxType{nullptr, None}, Expr
    };
    return E;
  }
}

auto Parser::ParseDecl() -> SyntaxExpression* {
  return nullptr;
}


/// Pratt Parsing Expression Technique (Precedence Matching)
/// References: https://en.wikipedia.org/wiki/Operator-precedence_parser
auto Parser::ParseExpression(int PreviousBinding) -> SyntaxExpression*  {
  auto LHS = Memory->Next<SyntaxExpression>(1);
  auto BeginToken = Curr();
  auto BeginKind = BeginToken->getKind();

  // ~ : Bitwise NOT
  // + : Positive
  // - : Negation
  // & : Memory Reference
  // * : Pointer Dereference
  auto UnaryBinding = Parser::UnaryOperatorBinding(BeginKind);
  if (UnaryBinding > PreviousBinding) {
    auto E = ParseExpression(UnaryBinding);
    const auto Op = SyntaxUnary::MatchOp(BeginKind);
    LHS->Unary = {
      Op, BeginToken, E
    };
  } else {
    // TODO: Match Complex Expressions
    // ()
    // match_call()
    // 323 or "sd" 23.23
    LHS = MatchExpression();
  }

  for (;;) {
    auto Op = Skip(1);
    auto OpKind = Op->getKind();

    if (OpKind == TokenKind::Dot) {
      if (Peek(1)->getKind() != TokenKind::Identifier)  {
        // TODO: Add Error Here
      }
    }

    const auto OpBindings = Parser::InfixOperatorBinding(OpKind);
    if (/* LeftBindingPrecedence */ OpBindings[0] <= PreviousBinding)
      break;
    Skip(1);
    const auto RHS = ParseExpression(/* RightBindingPrecedence */
      OpBindings[1]);
    auto NewExpression = Memory->Next<SyntaxExpression>(1);
    NewExpression->Binary = {
      SyntaxBinary::MatchOp(OpKind), LHS, RHS, Op
    };
    // Move Pointer to LHS
    LHS = NewExpression;
  }
  return LHS;
}

auto Parser::MatchExpression() -> SyntaxExpression * {
  auto BeginToken = Curr();
  switch (BeginToken->getKind()) {
    case EOFToken:
      // TODO: Handle Error Case
      break;
    case Identifier: {// Unresolved Variable or Function Call
      if (Peek(1)->getKind() == TokenKind::LParenthesis) {
        // This is a function call
        // auto E = ParseFunctionCallExpression()
        // return E;
      }
    }
    case Integer:
    case String:
    case Char:
    case Decimal: {
      auto E = Memory->Next<SyntaxExpression>(1);
      E->Literal = {
        BeginToken
      };
      return E;
    }
    case LParenthesis: {
      auto E = ParseExpression();
      Skip(TokenKind::RParenthesis, ParseErrorType::MissingClosingParenthesis);
      return E;
    }
    default: // TODO: Unexpected Expression Token
      break;
  }
}


auto Parser::ParseStatement() -> SyntaxExpression* {
  return nullptr;
}

auto Parser::ParseValueType() -> SyntaxType {
  return SyntaxType { nullptr, None };
}

auto Parser::ParseIfStatement() -> SyntaxExpression* {
  return nullptr;
}



