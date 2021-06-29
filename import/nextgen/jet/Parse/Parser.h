# ifndef JET_PARSER_H
# define JET_PARSER_H
# include "../Lex/Lexer.h"
# include "SyntaxNodes.h"

namespace nextgen { namespace jet { using namespace nextgen::core;

  enum ParseErrorType {
    ReservedIdentifierAsVariableName,
    UnexpectedExpression,
    InvalidToken,
    ExpectedToken,

    MissingFunctionName,
    MissingVariableName,

    ExpectedIdentifierForFunctionParameter,
    ExpectedIdentiierForStructProperty,

    MissingClosingParenthesis,
    MissingClosingCurlyBrace
  };

  enum ParseContext {
    AnonymousBlock,
    StatementBlock,
    LambdaBlock,
    StructBlock,
    MatchBlock,
    GeneralScope
  };

  struct ParseError {
    ParseErrorType Error;
    TokenTraits::SourceLocation Location;

    int Metadata[5];
  };

  class Parser {

    ParseContext    Context = GeneralScope;
    Diagnostic      ErrorBuilder;

    ArenaSegment *Memory;

    size_t Position = 0;
  public:


    mem::Vec<Token> Tokens;
    Parser() = default;

    Parser(Lexer *Lex) {
      Tokens = Lex->Lex();
      ErrorBuilder = Lex->ErrorBuilder;
      Memory = Lex->Memory;
    }


    /// Parse all statements
    void Parse();

    
    auto ParseDecl() -> SyntaxExpression*;
    auto ParseStatement() -> SyntaxExpression*;
    auto ParseExpression(int PreviousBinding = -1) -> SyntaxExpression*;
    auto MatchExpression() -> SyntaxExpression*;

    // ========= Parsing Specifics ==========

    auto ParseValueType()      -> Option<SyntaxType>;
    auto ParseFunctionParam()  -> Vec<SyntaxFunctionParameter>;
    auto ParseVariableDecl()   -> SyntaxExpression*;
    auto ParseFunctionDecl()   -> SyntaxExpression*;
    auto ParseStructDecl()     -> SyntaxExpression*;
    auto ParseIfStatement()    -> SyntaxExpression*;
    auto ParseForStatement()   -> SyntaxExpression*;
    auto ParseMatchStatement() -> SyntaxExpression*;
    auto ParseFunctionCall()   -> SyntaxExpression*;

    auto ParseBlock() -> SyntaxBlock;


    /// Lookahead 'NTok' amount of times in the list
    auto Peek(size_t NTok) -> Token*;

    /// Asserts that the value in 'Kind' is the value of the next token
    /// and otherwise builds a diagnostic instead for the 'ErrorKind'.
    auto Skip(TokenKind Kind, ParseErrorType ErrorKind) -> Token*;

    /// Skip 'NTok' number of tokens in the token stream.
    auto Skip(size_t NTok) -> Token*;


    /// Get the current token value
    NG_INLINE auto Curr() -> Token* {
      return Tokens[Position];
    }

    /// Get Unary Operator Precedence
    static NG_INLINE int UnaryOperatorBinding(TokenKind Kind) {
      switch (Kind) {
        case TokenKind::Plus:
        case TokenKind::Minus:
        case TokenKind::ExclamationPoint:
        case TokenKind::NOT:
          return 11;
        default:
          return -1;
      }
    }

    /// Generates Binary Expression Infix Operator Binding.
    /// Note Format: {LBP, RBP} for Operator Expression Binding.
    static NG_INLINE
      auto InfixOperatorBinding(TokenKind Kind) -> std::array<int, 2> {
      switch (Kind) {
        case TokenKind::Dot:
          return {10, 9}; // Property value operator
        case TokenKind::Star:
        case TokenKind::Slash:
        case TokenKind::Percent:
          return {7, 8};
        case TokenKind::Plus:
        case TokenKind::Minus:
          return {6, 7};
        case TokenKind::GreaterThan:
        case TokenKind::LessThan:
          return {5, 6};
        case TokenKind::EqualsEquals:
          return {4, 5};
        case TokenKind::KeywordAnd:
          return {3, 4};
        case TokenKind::KeywordOr:
          return {2, 3};
        case TokenKind::AND:
        case TokenKind::NOT:
        case TokenKind::XOR:
        case TokenKind::Pipe:
          return {1, 2};
        default:
          return {-1, -1};
      }
    }

  };
}}



# endif //JET_PARSER_H
