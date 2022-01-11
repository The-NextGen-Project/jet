#ifndef JET_ERROR_TYPES_H
#define JET_ERROR_TYPES_H

#include "parser-types.h"

namespace nextgen { namespace jet {

  enum Lex_Mode {
    TokenMode,
    PrintingMode
  };

  enum Lex_Error_Kind {
    // Other
    Unexpected_EOF,
    Out_Of_Range,
    Unreachable,

    // Strings
    Malformed_UTF8,
    Invalid_Char,
    Invalid_String_Escape,
    Invalid_Unicode_Escape_Close,
    Hex_Escape_Out_Of_Range,
    Missing_Closing_Delim,

    // Numbers
    Digit_Out_Of_Range,
    Invalid_Dot,
    Invalid_Number_Value,
    Integer_Overflow,
    Floating_Point_Overflow,
  };

  struct Lex_Error {
    // The type of error
    Lex_Error_Kind error;

    // Error location
    Source_Location location;

    // Token value lexed until error
    Token failed_token;

    // Error metadata
    int metadata[5];
  };


  struct Parse_Error {
    union Metadata;

    Parse_Error_Kind error;
    Source_Location location;
    std::initializer_list<Metadata> metadata;

  public: // Constructors
    Parse_Error(Parse_Error_Kind error, Source_Location loc)
      : error(error), location(loc) {}

    Parse_Error(Parse_Error_Kind error, Source_Location loc,
                const std::initializer_list<Metadata> &metadata)
      : error(error), location(loc), metadata(metadata) {}

    union Metadata {
      struct {
        Token_Kind expected;
        Token const *got;
        const char *message;
      } expected_error;
      Source_Location location;
      Token_Kind const misc_kind;
      Token const *misc_tok;
      const char *context_message;

    public: // Constructors

      // GCC and CLANG let us get away with not using union constructors,
      // but we need them for MSVC, and it's probably more clear anyway
      Metadata(Source_Location loc) : location(loc) {}
      Metadata(Token_Kind const kind) : misc_kind(kind) {}
      Metadata(Token const *tok) : misc_tok(tok) {}
      Metadata(const char *msg) : context_message(msg) {}
      Metadata(Token_Kind const expected, Token const *got, const char* message) {
        this->expected_error.expected = expected;
        this->expected_error.got = got;
        this->expected_error.message = message;
      }
    };
  };
}}



#endif //JET_ERROR_TYPES_H
