#ifndef JET_PARSER_H
#define JET_PARSER_H
#include "lexer.h"
#include "types/parser-types.h"

namespace nextgen::jet { using namespace nextgen::core;

  struct Parser;
  auto parse(Parser &self) -> const Parse_Result;


  struct Parser {
    int     fatal = 0;
    size_t  position = 0;
    Parse_Context       context;
    Diagnostic          diagnostics;
    std::vector<Token>  tokens;
  public: // Impl

    NG_AINLINE Token *curr()  {
      return &(tokens[position]);
    }

    [[nodiscard]] NG_AINLINE auto peek(size_t n) const {
      return &tokens[position + n];
    }

    NG_INLINE auto skip(size_t n) {
      auto *ret = &tokens[position];
      position += n;
      return ret;
    }

    NG_INLINE auto next(size_t n) {
      if (position + n >= tokens.size()) {
        position = tokens.size()-1;
      }
      else {
        position += n;
      }
      return &tokens[position];
    }

    template<Token_Kind TK, Parse_Error_Kind PE>
    NG_INLINE auto skip() -> const Token* {
      auto next = &tokens[(position++)];
      if (next->type() != TK) {
        this->diagnostics.build(Parse_Error(PE, next->location()));
      }
      return next;
    }

    template<Token_Kind TK, size_t N>
    NG_INLINE auto expect(char const (&msg)[N]) -> const Token* {
      const Token *next = &tokens[(position++)];
      if (next->type() != TK) {
        this->diagnostics.build(Parse_Error(
          Parse_Error_Kind::Expected_Token,
          next->location(),
          {
            Parse_Error::Metadata {TK, next, msg }
          }
        ));
        if (curr()->type() is EOFToken)
          this->diagnostics.send_exception();
        else
          fatal++;
      }
      return next;
    }

    template<Token_Kind TK>
    NG_INLINE void expect_delim(const Source_Location &loc) {
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

    template<Token_Kind TY>
    NG_AINLINE bool curr_not_eq() {
      return this->curr()->type() != TY and this->curr()->type() != EOFToken;
    }

  };
}


#endif //JET_PARSER_H