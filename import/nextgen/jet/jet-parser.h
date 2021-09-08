# ifndef JET_JET_PARSER_H
# define JET_JET_PARSER_H
# include "jet-lexer.h"
# include "jet-syntax-nodes.h"

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

      MissingClosingPair,

      UnexpectedEndOfFile
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
      ParseErrorType error;
      TokenTraits::SourceLocation location;

      // Error specifics that are handled by
      // the [Diagnostic] class.
      union Metadata {
        struct {
          TokenKind expected;
          TokenKind got;
          const char *message;
        } expected_error;
        TokenTraits::SourceLocation location;
        TokenKind reserved_ident;

        Metadata(TokenTraits::SourceLocation loc) : location(loc) {}
        Metadata(TokenKind reserved_ident) : reserved_ident(reserved_ident) {}
        Metadata(TokenKind expected, TokenKind got, const char *message) {
          expected_error.expected = expected;
          expected_error.got = got;
          expected_error.message = message;
        }
      };

      std::initializer_list<Metadata> metadata;

      ParseError(ParseErrorType error, TokenTraits::SourceLocation loc)
        : error(error), location(loc) {}

      ParseError(ParseErrorType error, TokenTraits::SourceLocation loc,
                 const std::initializer_list<Metadata> &metadata)
        : error(error), location(loc) {}
    };

    class Parser {

      // Parser's position
      size_t position = 0;

      // Parsing context (valid statements per context scope)
      ParseContext    context = GeneralScope;

      // Parsing errors
      Diagnostic      diagnostics;

      // Encountered error that results in stopped compilation
      char fatal = 0;
    public:
      Token *tokens;
      Parser() = default;

      explicit Parser(Lexer<TokenMode> *lexer)
        : tokens(lexer->lex()), diagnostics(lexer->diagnostics),
          fatal(0) {}

      ~Parser() {
        vec::clear(tokens);
      }


      /// Parse all statements
      void parse();

      // ========= Parsing Language Generalizations ==========

      auto parse_decl() -> SyntaxNode*;
      auto parse_stmt() -> SyntaxNode*;
      auto parse_expr(int previous_binding = -1) -> SyntaxNode*;
      auto match_expr() -> SyntaxNode*;

      // ========= Parsing Language Constructs ==========

      auto parse_block()           -> SyntaxBlock;
      auto parse_type()            -> SyntaxType;
      auto parse_function_param(SyntaxFunction *func)  -> void;
      auto parse_variable_decl()   -> SyntaxNode*;
      auto parse_function()        -> SyntaxNode*;
      auto parse_struct()          -> SyntaxNode*;
      auto parse_if()              -> SyntaxNode*;
      auto parse_for()             -> SyntaxNode*;
      auto parse_match()           -> SyntaxNode*;
      auto parse_function_call()   -> SyntaxNode*;
      auto parse_struct_function
        (Token *s, Token *function_name) -> SyntaxNode*;


      // ========= Parsing Utils ==========

      /// Lookahead 'n' amount of times in the list
      auto peek(size_t n) -> Token*;

      /// Returns the current token and skips 'n' tokens ahead
      auto skip(size_t n) -> Token*;

      /// Skips the 'n' and returns the latest token after the skip
      auto next(size_t n) -> Token*;

      /// Asserts that the value in 'kind' is the value of the next token
      /// and otherwise builds a diagnostic instead for the 'error_kind'.
      template<TokenKind TK, ParseErrorType PE>
      auto skip() -> Token*;

      /// Expect the next token to be 'kind', if not, build an error. A lot of
      /// tokens are expected, and individual errors cannot be based on every
      /// situation.
      template<TokenKind TK, size_t N>
      auto expect(char const (&msg)[N]) -> Token*;

      template<TokenKind TK>
      void delim(const TokenTraits::SourceLocation &start);

      /// Get the current token value
      NG_INLINE Token *curr()  {
        return tokens + position;
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


# endif //JET_JET_PARSER_H