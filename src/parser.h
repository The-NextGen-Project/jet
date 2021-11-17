# ifndef JET_PARSER_H
# define JET_PARSER_H
# include "lexer.h"
# include "types/parser-types.h"

namespace nextgen { namespace jet { using namespace nextgen::core;

  class Parser {
    int     fatal = 0;
    size_t  position = 0;
    Parse_Context       context;
    Diagnostic          diagnostics;
    std::vector<Token>  tokens;
  public:
    Parser() = default;

    // Why are we passing a pointer here? Each pass in the frontend is done
    // through the main function, thus, when we go ahead and pass the lexer,
    // we know that it has the lifetime of the program.
    explicit Parser(Lexer<TokenMode> *lexer)
      : tokens(lexer->lex()), diagnostics(lexer->diagnostics),
        fatal(0) {}

    /// Parse all statements
    auto parse() -> const Parse_Result;

    // ========= Parsing Language Generalizations ==========
    NG_AINLINE Token *curr()  { return &(tokens[position]); }

    // ========= Parsing Utils ==========

    /** @brief Lookahead 'n' amount of times in the list */
    auto NG_AINLINE peek(size_t n) {
      return &tokens[position + n];
    }

    /** @brief Returns the current token and skips 'n' tokens ahead */
    auto NG_INLINE skip(size_t n) {
      auto *ret = &tokens[position];
      position += n;
      return ret;
    }

    /** @brief Skips 'n' tokens and returns the latest token after the skip */
    auto next(size_t n) {
      if (position + n >= tokens.size()) {
        position = tokens.size()-1;
      }
      else {
        position += n;
      }
      return &tokens[position];
    }

    /** @brief Asserts next token kind is 'TK' or errors with type 'PE' */
    template<Token_Kind TK, Parse_Error_Kind PE>
    auto skip() -> const Token* {
      auto next = &tokens[(position++)];
      if (next->type() != TK) {
        this->diagnostics.build(Parse_Error(
          PE,
          next->location()
          ));
      }
      return next;
    }

    /** @brief Expect the next token to be 'kind', if not, build an error. */
    template<Token_Kind TK, size_t N>
    auto expect(char const (&msg)[N]) -> const Token* {
      const Token *next = &tokens[(position++)];
      if (next->type() != TK) {
        this->diagnostics.build(Parse_Error(
          Parse_Error_Kind::Expected_Token,
          next->location(),
          {
            Parse_Error::Metadata {TK, next, msg }
          }
          ));
        if (curr()->type() == EOFToken)
          this->diagnostics.send_exception();
        else
          fatal++;
      }
      return next;
    }

    /** @brief Expected a closing delim in a statement or expression */
    template<Token_Kind TK>
    void expect_delim(const Source_Location &loc) {
      Token *next = &tokens[position++];
      if (next->type() != TK) {
        this->diagnostics.build(Parse_Error (
          Parse_Error_Kind::Missing_Closing_Pair,
          next->location(),
          {
            Parse_Error::Metadata {TK},
            Parse_Error::Metadata {next},
            Parse_Error::Metadata {loc}
          }
          ));

        // We cannot recover from a mistake in an expression or pair
        this->diagnostics.send_exception();
      }
    }


    /** @brief Unary operator precedence */
    static NG_INLINE int UnaryOperatorBinding(Token_Kind kind) {
      switch (kind) {
        case Token_Kind::Plus:
        case Token_Kind::Minus:
        case Token_Kind::Exclamation_Point:
        case Token_Kind::NOT:
          return 17;
        default:
          return -1;
      }
    }

    /** @brief Infix operator binding for expressions */
    static NG_INLINE
    std::array<int, 2> InfixOperatorBinding(Token_Kind kind) {
      switch (kind) {
        case Token_Kind::Dot:
          return {16, 15}; // Property value operator
        case Token_Kind::Star:
        case Token_Kind::Slash:
        case Token_Kind::Percent:
          return {13, 14};
        case Token_Kind::Plus:
        case Token_Kind::Minus:
          return {11, 12};
        case Token_Kind::Greater_Than:
        case Token_Kind::Less_Than:
          return {9, 10};
        case Token_Kind::Equals_Equals:
          return {7, 8};
        case Token_Kind::Keyword_And:
          return {5, 6};
        case Token_Kind::Keyword_Or:
          return {3, 4};
        case Token_Kind::AND:
        case Token_Kind::NOT:
        case Token_Kind::XOR:
        case Token_Kind::Pipe:
          return {1, 2};
        default:
          return {-1, -1};
      }
    }
    auto parse_expr(int previous_binding = -1) -> const Syntax_Node*;
    auto match_expr() -> const Syntax_Node*;

    // ========= Parsing Language Constructs ==========

    auto parse_block(bool ret = false) -> Syntax_Block;
    auto parse_type() -> Syntax_Type*;
    auto parse_function_param() -> std::vector<Syntax_Function_Parameter>;
    auto parse_variable_assignment(const Token *name) -> const Syntax_Node*;
    auto parse_function(const Token *name) -> const Syntax_Function*;
    auto parse_struct_data_members() -> std::vector<Syntax_Struct_Member>;
    auto parse_function_call(const Token *name, const Token *delim) -> const Syntax_Node*;


    auto parse_for() -> const Syntax_Node*;
    auto parse_match() -> const Syntax_Node*;
    auto parse_match_pair_value() -> const Syntax_Node*;

    auto parse_path(const Token *type) -> const Syntax_Node*;
    auto parse_struct_instantiation(const Token *name) -> const Syntax_Node*;

    auto parse_enum(const Token *name) -> const Syntax_Enum*;

    //========== INLINED FUNCTIONS ==========


    template<bool ELIF = false>
    auto parse_if() -> const Syntax_Node* {
      auto cond = parse_expr();
      auto body = parse_block();

      Syntax_Else *else_ = nullptr; Syntax_Elif *elif = nullptr;
      auto is_else = next(1);
      if (is_else->type() == Token_Kind::Keyword_Else) {
        else_ = new Syntax_Else(parse_block());
      }
      else if (is_else->type() == Token_Kind::Keyword_Elif) {
        elif = (Syntax_Elif*)((Syntax_Node*) parse_if());
      }
      auto E = (Syntax_Node*) new Syntax_If(cond, body, else_, elif);
      if constexpr (ELIF)
        E->kind = Syntax_Kind::Elif;
      return E;
    }

    auto NG_INLINE
    parse_while() {
      auto cond = parse_expr();
      auto body = parse_block();
      return new Syntax_While(cond, body);
    }

    auto NG_INLINE
    parse_variable_value_assignment(const Token *name, Syntax_Assignment_Op op)  {
      auto ret = new Syntax_Variable_Value_Assignment(name, parse_expr(), op);
      expect<Token_Kind::Semi_Colon>("Expected ';' after declaration");
      return ret;
    }

    auto parse_struct(const Token *name) -> const Syntax_Struct* {
      expect<Token_Kind::LCurly_Brace>("Opening curly brace is required "
                                     "before struct member declarations");
      return new Syntax_Struct(name, parse_struct_data_members());
    }

    auto parse_stmt() -> const Syntax_Node* {
      switch (curr()->type()) {
        case Keyword_If:
          skip(1);
          return parse_if();
        case Keyword_While:
          context = Parse_Context::Loop_Block;
          skip(1);
          return parse_while();
        case Keyword_For:
          context = Parse_Context::Loop_Block;
          skip(1);
          return parse_for();
        case Keyword_Break: {
          if (context != Parse_Context::Loop_Block) {
            // TODO: Add Error
          }
          break;
        }
        case Keyword_Continue: {
          if (context != Parse_Context::Loop_Block) {
            // TODO: Add Error
          }
          break;
        }
        case Keyword_Defer:
          break;
        case Keyword_None:
          break;
        case Keyword_Return: {
          auto value = parse_expr();
          return new Return(value);
        }
        case Keyword_Match:
          break;
        case Identifier: {
          auto C1 = peek(1);
          if (C1->type() == Token_Kind::Colon_Equals) {
            auto name = curr();
            skip(2);
            return parse_variable_assignment(name);
          }
          if (C1->is_value_assignment_op()) {
            auto name = curr();
            skip(2);
            return parse_variable_value_assignment(name,
                                                   Syntax_Variable_Value_Assignment::MatchOp(C1->type()));
          }
          // TODO: Add Path Value Check
          break;
        }
        case LCurly_Brace:
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


# endif //JET_PARSER_H