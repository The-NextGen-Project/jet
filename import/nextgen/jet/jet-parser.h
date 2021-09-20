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
      union Metadata;

      ParseErrorType error;
      TokenTraits::SourceLocation location;
      std::initializer_list<Metadata> metadata;

      /** @brief Contains specific error information pertaining to the error
       * message. Only a few parser errors have specific items, while most are
       * invalid or missing tokens
       * */
      union Metadata {
        struct {
          TokenKind expected;
          Token const *got;
          const char *message;
        } expected_error;
        TokenTraits::SourceLocation location;
        TokenKind const misc_kind;

        // GCC and CLANG let us get away with not using union constructors,
        // but we need them for MSVC, and it's probably more clear anyway
        Metadata(TokenTraits::SourceLocation loc) : location(loc) {}
        Metadata(TokenKind const kind) : misc_kind(kind) {}
        Metadata(TokenKind const expected, Token const *got, const char
        *message) {
          expected_error.expected = expected;
          expected_error.got = got;
          expected_error.message = message;
        }
      };

      ParseError(ParseErrorType error, TokenTraits::SourceLocation loc)
        : error(error), location(loc) {}

      ParseError(ParseErrorType error, TokenTraits::SourceLocation loc,
                 const std::initializer_list<Metadata> &metadata)
        : error(error), location(loc), metadata(metadata) {}
    };

    class Parser {

      // Parser's position
      size_t position = 0;

      // Parsing context (valid statements per context scope)
      // TODO: Set this up so stuff actually works with this...
      ParseContext    context = GeneralScope;

      // Parsing errors
      Diagnostic      diagnostics;

      // Encountered error that results in stopped compilation
      char fatal = 0;


    public:
      ArenaVec<Token> tokens;
      Parser() = default;

      // Why are we passing a pointer here? Each pass in the frontend is done
      // through the main function, thus, when we go ahead and pass the lexer,
      // we know that it has the lifetime of the program.
      explicit Parser(Lexer<TokenMode> *lexer)
        : tokens(lexer->lex()), diagnostics(lexer->diagnostics),
          fatal(0) {}

      /// Parse all statements
      void parse();

      // ========= Parsing Language Generalizations ==========

      auto parse_decl() -> SyntaxNode*;
      auto parse_stmt() -> SyntaxNode*;
      auto parse_expr(int previous_binding = -1) -> SyntaxNode*;
      auto match_expr() -> SyntaxNode*;

      // ========= Parsing Language Constructs ==========

      auto parse_block() -> SyntaxBlock;
      auto parse_type() -> SyntaxType;
      void parse_function_param(SyntaxFunction *func);
      auto parse_variable_assignment(Token *name) -> SyntaxNode*;
      auto parse_function(Token *name) -> SyntaxNode*;
      auto parse_struct(Token *name) -> SyntaxNode*;
      auto parse_function_call(Token *name, Token *delim) -> SyntaxNode*;
      auto parse_if() -> SyntaxNode*;
      auto parse_for() -> SyntaxNode*;
      auto parse_match() -> SyntaxNode*;
      auto parse_match_pair_value() -> SyntaxNode*;
      auto parse_while() -> SyntaxNode*;

      // TODO: Update in design choices, do we need this still?
      auto parse_struct_function
        (Token *s, Token *function_name) -> SyntaxNode*;


      // ========= Parsing Utils ==========

      /** @brief Lookahead 'n' amount of times in the list */
      auto peek(size_t n) -> Token*;

      /** @brief Returns the current token and skips 'n' tokens ahead */
      auto skip(size_t n) -> Token*;

      /** @brief Skips 'n' tokens and returns the latest token after the skip */
      auto next(size_t n) -> Token*;

      /** @brief Asserts next token kind is 'TK' or errors with type 'PE' */
      template<TokenKind TK, ParseErrorType PE>
      auto skip() -> Token*;

      /** @brief Expect the next token to be 'kind', if not, build an error. */
      template<TokenKind TK, size_t N>
      auto expect(char const (&msg)[N]) -> Token*;

      /** @brief Expected a closing delim in a statement or expression */
      template<TokenKind TK>
      void expect_delim(const TokenTraits::SourceLocation &loc);


      /** @brief Unary operator precedence */
      static NG_INLINE int UnaryOperatorBinding(TokenKind kind) {
        switch (kind) {
          case TokenKind::Plus:
          case TokenKind::Minus:
          case TokenKind::ExclamationPoint:
          case TokenKind::NOT:
            return 17;
          default:
            return -1;
        }
      }

      /** @brief Infix operator binding for expressions */
      static NG_INLINE
      std::array<int, 2> InfixOperatorBinding(TokenKind kind) {
        switch (kind) {
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

      auto NG_AINLINE curr()  { return tokens[position]; }
    };
  }}


# endif //JET_JET_PARSER_H