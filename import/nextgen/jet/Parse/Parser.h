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
      MissingForLoopVariable,

      ExpectedIdentifierForFunctionParameter,
      ExpectedIdentifierForStructProperty,

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

      // Error specifics that are handled by
      // the [Diagnostic] class.
      union Metadata {
        struct {
          TokenKind expected;
          TokenKind got;
        } expected_error;
        TokenKind reserved_ident;

        Metadata(TokenKind reserved_ident) : reserved_ident(reserved_ident) {}
        Metadata(TokenKind expected, TokenKind got) {
          expected_error.expected = expected;
          expected_error.got = got;
        }
      };

      std::initializer_list<Metadata> metadata;

      ParseError(ParseErrorType error, TokenTraits::SourceLocation loc)
      : Error(error), Location(loc) {}

      ParseError(ParseErrorType error, TokenTraits::SourceLocation loc,
                 const std::initializer_list<Metadata> &metadata)
                 : Error(error), Location(loc) {}
    };

    class Parser {

      // Parser's position
      size_t position = 0;

      // Parsing context (valid statements per context scope)
      ParseContext    context = GeneralScope;

      // Parsing errors
      Diagnostic      diagnostics;

      // Parser allocator
      ArenaSegment    *allocator;

      // Encountered error that results in stopped compilation
      bool fatal = false;
    public:


      mem::Vec<Token> tokens;
      Parser() = default;

      explicit Parser(Lexer *lexer)
        : tokens(lexer->lex()), diagnostics(lexer->diagnostics),
          allocator(lexer->allocator), fatal(false) {}



      /// Parse all statements
      void parse();


      auto parse_decl() -> SyntaxNode*;
      auto parse_stmt() -> SyntaxNode*;
      auto parse_expr(int previous_binding = -1) -> SyntaxNode*;
      auto match_expr() -> SyntaxNode*;

      // ========= Parsing Specifics ==========

      auto parse_block()           -> SyntaxBlock;
      auto parse_type()            -> Option<SyntaxType>;
      auto parse_function_param()  -> Vec<SyntaxFunctionParameter>;
      auto parse_variable_decl()   -> SyntaxNode*;
      auto parse_function()        -> SyntaxNode*;
      auto parse_struct()          -> SyntaxNode*;
      auto parse_if()              -> SyntaxNode*;
      auto parse_for()             -> SyntaxNode*;
      auto parse_match()           -> SyntaxNode*;
      auto parse_function_call()   -> SyntaxNode*;
      auto parse_struct_function
        (Token *s, Token *function_name) -> SyntaxNode*;


      /// Lookahead 'n' amount of times in the list
      auto peek(size_t n) -> Token*;

      /// Asserts that the value in 'kind' is the value of the next token
      /// and otherwise builds a diagnostic instead for the 'error_kind'.
      auto skip(TokenKind kind, ParseErrorType error_kind) -> Token*;

      /// Returns the current token and skips 'n' tokens ahead
      auto skip(size_t n) -> Token*;

      /// Skips the 'n' and returns the latest token after the skip
      auto next(size_t n) -> Token*;

      /// Expect the next token to be 'kind', if not, build an error. A lot of
      /// tokens are expected, and individual errors cannot be based on every
      /// situation.
      int expect(TokenKind kind);

      /// Get the current token value
      NG_INLINE Token *curr()  {
        return tokens.get_pointer_at(position);
      }

      /// Get Unary Operator Precedence
      static NG_INLINE int UnaryOperatorBinding(TokenKind Kind) {
        switch (Kind) {
          case TokenKind::Plus:
          case TokenKind::Minus:
          case TokenKind::ExclamationPoint:
          case TokenKind::NOT:
            return 17;
          default:
            return -1;
        }
      }

      /// Generates Binary Expression Infix Operator Binding.
      /// Note Format: {LBP, RBP} for Operator Expression Binding.
      static NG_INLINE
      auto InfixOperatorBinding(TokenKind Kind) -> std::array<int, 2>  {
        switch (Kind) {
          case TokenKind::Dot:
            return {16, 15}; // Property value operator
          case TokenKind::Star:
          case TokenKind::Slash:
          case TokenKind::Percent:
            return {13, 14};
          case TokenKind::Plus:
          case TokenKind::Minus:
            return {11, 12};
          case TokenKind::GreaterThan:
          case TokenKind::LessThan:
            return {9, 10};
          case TokenKind::EqualsEquals:
            return {7, 8};
          case TokenKind::KeywordAnd:
            return {5, 6};
          case TokenKind::KeywordOr:
            return {3, 4};
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