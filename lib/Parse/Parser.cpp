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



# include <nextgen/Jet/Parse/Parser.h>

using namespace nextgen::jet;

void Parser::Parse() {
  switch (Curr()->getKind()) {
    case TokenKind::EOFToken:
      break;
    case TokenKind::KeywordVar:
      Skip(1);
      ParseVariableDecl();
      break;
    case TokenKind::KeywordStruct:
      Skip(1);
      ParseStructDecl();
      break;
    case TokenKind::KeywordFunction:
      Skip(1);
      ParseFunctionDecl();
      break;
    case TokenKind::KeywordIf:
      Skip(1);
      ParseIfStatement();
      break;
    case TokenKind::KeywordFor:
      Skip(1);
      ParseForStatement();
      break;
    case TokenKind::KeywordMatch:
      Skip(1);
      ParseMatchStatement();
      break;
    case TokenKind::Identifier: {
      auto C1 = Peek(1);
      auto C2 = Peek(2);
      auto C3 = Peek(3);

      // TODO: What if we are in generics?
      if (C1->getKind() == TokenKind::Dot) {
        if (C2->getKind() == TokenKind::Identifier) {
          if (C3->getKind() == TokenKind::Equals) {
            // Ok, we are in special struct function
            Skip(3);
            ParseStructFunctionDecl(Curr(), C2);
          }
        } else ParseExpression();
      } else ParseExpression();
      break;
    }
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
    E->VariableAssignment = SyntaxVariableAssignment {
      Name, None, Expr
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
  // * : pointer Dereference
  auto UnaryBinding = Parser::UnaryOperatorBinding(BeginKind);
  if (UnaryBinding > PreviousBinding) {
    auto E = ParseExpression(UnaryBinding);
    const auto Op = SyntaxUnary::MatchOp(BeginKind);
    LHS->Unary = {
      Op, BeginToken, E
    };
  } else {
    LHS = MatchExpression();
  }

  for (;;) {
    auto Op = Skip(1);
    auto OpKind = Op->getKind();

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
    NewExpression->Kind = SyntaxKind::Binary;
    // Move pointer to LHS
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
    case Decimal:
    case KeywordTrue:
    case KeywordFalse: {
      auto E = Memory->Next<SyntaxExpression>(1);
      E->Literal = {
        BeginToken
      };
      E->Kind = SyntaxKind::LiteralValue;
      return E;
    }
    case LParenthesis: {
      auto E = ParseExpression();
      Skip(TokenKind::RParenthesis, ParseErrorType::MissingClosingParenthesis);
      return E;
    }
    case LBracket: { // ie: [1, 2, 3]
      auto Current = Skip(1);
      auto Values = Vec<SyntaxExpression*>(5);
      while (Current->getKind() != TokenKind::RBracket) {
        if (Current->getKind() == TokenKind::EOFToken) {
          // TODO: Add Error
        }
        Values.push(ParseExpression());
        Current = Skip(TokenKind::Comma,
                     ParseErrorType::MissingClosingCurlyBrace);
      }
      auto E  = Memory->Next<SyntaxExpression>(1);
      E->Kind = SyntaxKind::List;
      E->List = SyntaxList { Values };
      return E;
    }
    default: // TODO: Unexpected Expression Token
      break;
  }
  return nullptr;
}


auto Parser::ParseStatement() -> SyntaxExpression* {
  return nullptr;
}

auto Parser::ParseValueType() -> Option<SyntaxType> {
  if (Peek(1)->getKind() == TokenKind::Comma)
    return None;
}

auto Parser::ParseIfStatement() -> SyntaxExpression* {
  auto Cond = ParseExpression();

  SyntaxBlock Body;
  SyntaxExpression *Else = nullptr;
  SyntaxExpression *Elif = nullptr;

  Body = ParseBlock();
  auto IsElse = Skip(1);
  if (IsElse->getKind() == TokenKind::KeywordElse) {
    SyntaxBlock ElseBody = ParseBlock();
    Else = Memory->Next<SyntaxExpression>(1);
    Else->Kind = SyntaxKind::Else;
    Else->Else = {
      ElseBody
    };
  }
  else if (IsElse->getKind() == TokenKind::KeywordElif) {
    Elif = ParseIfStatement();
    Elif->Kind = SyntaxKind::Elif;
  }

  auto E = Memory->Next<SyntaxExpression>(1);
  E->Kind = SyntaxKind::If;
  E->If = {
    Cond, Body, Else, Elif
  };
}

auto Parser::ParseForStatement() -> SyntaxExpression * {
  auto LoopVar = Skip(TokenKind::Identifier,
                      ParseErrorType::MissingForLoopVariable);
  Skip(TokenKind::KeywordIn, ParseErrorType::ExpectedToken);

  auto Begin = Skip(1);
}

auto Parser::ParseMatchStatement() -> SyntaxExpression * {
  return nullptr;
}

auto Parser::ParseFunctionParam() -> Vec<SyntaxFunctionParameter> {
  Skip(TokenKind::LParenthesis, ParseErrorType::ExpectedToken);
  auto Current = Curr();
  auto Params  = Vec<SyntaxFunctionParameter>{};
  while (Current->getKind() != TokenKind::RParenthesis) {
    auto ParamName = Skip(TokenKind::Identifier,
                          ParseErrorType::ExpectedIdentifierForFunctionParameter);
    Params.push({
                  ParamName, ParseValueType()
                });
  }
  return Params;
}

auto Parser::ParseFunctionDecl() -> SyntaxExpression * {
  auto E = Memory->Next<SyntaxExpression>(1);
  auto FunctionName = Skip(TokenKind::Identifier,
                           ParseErrorType::MissingFunctionName);

  auto Params       = ParseFunctionParam();
  auto FunctionType = ParseValueType();
  auto FunctionBody = ParseBlock();

  E->Kind = SyntaxKind::FunctionDefault;
  E->Function = {
    FunctionName, FunctionType, FunctionBody, Params
  };
  return nullptr;
}

auto Parser::ParseStructDecl() -> SyntaxExpression * {
  return nullptr;
}

auto Parser::ParseFunctionCall() -> SyntaxExpression * {
  auto FunctionName = Skip(1); // Guaranteed to be IDENT
  auto Params = Vec<SyntaxExpression*>{};
  Skip(TokenKind::LParenthesis, ParseErrorType::ExpectedToken);
  while(Curr()->getKind() != TokenKind::RParenthesis) {
    Params.push(ParseExpression());
  }
  Skip(TokenKind::RParenthesis, ParseErrorType::ExpectedToken);

  auto E = Memory->Next<SyntaxExpression>(1);
  E->Kind = SyntaxKind::FunctionCall;
  E->FunctionCall = {
    FunctionName, Params
  };
  return E;
}

auto Parser::ParseBlock() -> SyntaxBlock {
  Skip(TokenKind::LCurlyBrace, ParseErrorType::ExpectedToken);

  auto Current = Curr();
  auto Statements = Vec<SyntaxExpression*>{};
  while (Current->getKind() != TokenKind::RCurlyBrace) {
    Statements.push(ParseStatement());
  }
  Skip(TokenKind::RCurlyBrace, ParseErrorType::ExpectedToken);
  return {
    Statements
  };
}

auto Parser::ParseStructFunctionDecl(Token *Struct, Token *FuncName) ->
SyntaxExpression
* {

}



