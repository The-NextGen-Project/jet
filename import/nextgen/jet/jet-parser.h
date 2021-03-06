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
      InvalidTokenAfterIdentInGlobalScope,

      MissingClosingPair,
      UnexpectedEndOfFile
    };

    enum ParseContext {
      AnonymousBlock,
      StatementBlock,
      LoopBlock,
      LambdaBlock,
      GeneralScope
    };


    struct ParserOutput {
      ObjectVector<const SyntaxFunction*, 100> functions;
      ObjectVector<const SyntaxStruct*, 100> structures;
      ObjectVector<const SyntaxVariableAssignment*> global_variables;
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
        Token const *misc_tok;
        const char *context_message;

        // GCC and CLANG let us get away with not using union constructors,
        // but we need them for MSVC, and it's probably more clear anyway
        Metadata(TokenTraits::SourceLocation loc) : location(loc) {}
        Metadata(TokenKind const kind) : misc_kind(kind) {}
        Metadata(Token const *tok) : misc_tok(tok) {}
        Metadata(const char *msg) : context_message(msg) {}
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
      auto parse() -> const ParserOutput;

      // ========= Parsing Language Generalizations ==========
      auto NG_AINLINE curr()  { return tokens[position]; }

      // ========= Parsing Utils ==========

      /** @brief Lookahead 'n' amount of times in the list */
      auto NG_AINLINE peek(size_t n) {
        return tokens[position + n];
      }

      /** @brief Returns the current token and skips 'n' tokens ahead */
      auto NG_INLINE skip(size_t n) {
        auto *ret = tokens[position];
        position += n;
        return ret;
      }

      /** @brief Skips 'n' tokens and returns the latest token after the skip */
      auto NG_INLINE next(size_t n) {
        position += n;
        return tokens[position];
      }

      /** @brief Asserts next token kind is 'TK' or errors with type 'PE' */
      template<TokenKind TK, ParseErrorType PE>
      auto skip() -> const Token* {
        auto next = tokens[(position++)];
        if (next->getKind() != TK) {
          this->diagnostics.build(ParseError(
            PE,
            next->getSourceLocation()
            ));
        }
        return next;
      }

      /** @brief Expect the next token to be 'kind', if not, build an error. */
      template<TokenKind TK, size_t N>
      auto expect(char const (&msg)[N]) -> const Token* {
        const Token *next = tokens[(position++)];
        if (next->getKind() != TK) {
          this->diagnostics.build(ParseError(
            ParseErrorType::ExpectedToken,
            next->getSourceLocation(),
            {
              ParseError::Metadata { TK, next, msg }
            }
            ));
          if (curr()->getKind() == EOFToken)
            this->diagnostics.send_exception();
          else
            fatal++;
        }
        return next;
      }

      /** @brief Expected a closing delim in a statement or expression */
      template<TokenKind TK>
      void expect_delim(const TokenTraits::SourceLocation &loc) {
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
      auto parse_expr(int previous_binding = -1) -> const SyntaxNode*;
      auto match_expr() -> const SyntaxNode*;

      // ========= Parsing Language Constructs ==========

      auto parse_block() -> SyntaxBlock;
      auto parse_type() -> SyntaxType*;
      auto parse_function_param() -> ArenaVec<SyntaxFunctionParameter>;
      auto parse_variable_assignment(const Token *name) -> const SyntaxNode*;

      auto parse_function(const Token *name) -> const SyntaxFunction*;
      auto parse_struct_data_members() -> ArenaVec<SyntaxStructMember>;
      auto parse_function_call(const Token *name, const Token *delim) -> const SyntaxNode*;

      auto parse_for() -> const SyntaxNode*;
      auto parse_match() -> const SyntaxNode*;
      auto parse_match_pair_value() -> const SyntaxNode*;

      //========== INLINED FUNCTIONS ==========


      template<bool ELIF = false>
      auto parse_if() -> const SyntaxNode* {
        auto cond = parse_expr();
        auto body = parse_block();

        SyntaxElse *else_ = nullptr; SyntaxElif *elif = nullptr;
        auto is_else = next(1);
        if (is_else->getKind() == TokenKind::KeywordElse) {
          else_ = new SyntaxElse(parse_block());
        }
        else if (is_else->getKind() == TokenKind::KeywordElif) {
          elif = (SyntaxElif*)((SyntaxNode*) parse_if());
        }
        auto E = (SyntaxNode*) new SyntaxIf(cond, body, else_, elif);
        if (ELIF)
          E->kind = SyntaxKind::Elif;
        return E;
      }

      auto NG_INLINE
      parse_while() {
        auto cond = parse_expr();
        auto body = parse_block();
        return new SyntaxWhile(cond, body);
      }

      auto NG_INLINE
      parse_variable_value_assignment(const Token *name, SyntaxAssignmentOp op)  {
        auto ret = new SyntaxVariableValueAssignment(name, parse_expr(), op);
        expect<TokenKind::SemiColon>("Expected ';' after declaration");
        return ret;
      }

      auto parse_struct(const Token *name) -> const SyntaxStruct* {
        expect<TokenKind::LCurlyBrace>("Opening curly brace is required "
                                       "before struct member declarations");
        return new SyntaxStruct(name, parse_struct_data_members());
      }

      auto parse_stmt() -> const SyntaxNode* {
        switch (curr()->getKind()) {
          case KeywordIf:
            skip(1);
            return parse_if();
          case KeywordWhile:
            context = ParseContext::LoopBlock;
            skip(1);
            return parse_while();
          case KeywordFor:
            context = ParseContext::LoopBlock;
            skip(1);
            return parse_for();
          case KeywordBreak: {
            if (context != ParseContext::LoopBlock) {
              // TODO: Add Error
            }
            break;
          }
          case KeywordContinue: {
            if (context != ParseContext::LoopBlock) {
              // TODO: Add Error
            }
            break;
          }
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
            if (C1->isValueAssignmentOp()) {
              auto name = curr();
              skip(2);
              return parse_variable_value_assignment(name,
                                                     SyntaxVariableValueAssignment::MatchOp(C1->getKind()));
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
        next(1);
        return nullptr;
      }

    };
  }}


# endif //JET_JET_PARSER_H