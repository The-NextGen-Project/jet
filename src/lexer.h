#ifndef NEXTGEN_LEXER_H
#define NEXTGEN_LEXER_H

#include "types/lexer-types.h"
#include "diagnostics.h"
#include "allocator.h"

namespace nextgen::jet {

#define TOKEN_SHORT(value, kind) \
new Token { \
  value, \
  {L.line, L.column}, \
  value, \
  kind \
}
#define TOKEN_ADD_SHORT(value, kind) \
if constexpr (Mode is PrintingMode)           \
    Console::Log(value), L.next<Mode>((sizeof value) - 1); \
else                                 \
  (L.tokens.end = TOKEN_SHORT(value, kind), L.next<Mode>((sizeof value) - 1)) \
                                  \
                                     
#define NEXT L.next<Mode>(1)
  
  
  static const constexpr Token_Kind MatchTokenKind[256] {

    // Error code
    Token_Kind::EOFToken, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error,
    Token_Kind::Whitespace, // \a
    Token_Kind::Whitespace, // \b
    Token_Kind::Whitespace, // \t
    Token_Kind::NewLine,    // \n
    Token_Kind::Whitespace, // \v
    Token_Kind::Error,

    Token_Kind::NewLine, // \r

    Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error,


    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,

    Token_Kind::Whitespace,    // <space>

    Token_Kind::Exclamation_Point, // !
    Token_Kind::String,        // '"'
    Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Percent,       // '%'
    Token_Kind::AND,           // '&'
    Token_Kind::Char,          // '\''
    Token_Kind::LParenthesis,  // (
    Token_Kind::RParenthesis,  // )
    Token_Kind::Star,          // '*'
    Token_Kind::Plus,          // '+'
    Token_Kind::Comma,         // ','

    Token_Kind::Minus, // '-'


    Token_Kind::Dot, // '.'

    Token_Kind::Slash, // '/'


    // Digits
    Token_Kind::Zero, Token_Kind::Integer, Token_Kind::Integer,
    Token_Kind::Integer, Token_Kind::Integer,
    Token_Kind::Integer, Token_Kind::Integer, Token_Kind::Integer,
    Token_Kind::Integer, Token_Kind::Integer,


    Token_Kind::Colon, // ':'


    Token_Kind::Semi_Colon, // ';'
    Token_Kind::Less_Than, // '<'
    Token_Kind::Equals, // '='
    Token_Kind::Greater_Than, // '>'
    Token_Kind::Question_Mark, // '?'
    Token_Kind::At, // @

    // Uppercase letters
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier,


    Token_Kind::LBracket,  // '['

    Token_Kind::String_Escape_Start, // \\

    Token_Kind::RBracket,  // ']'

    Token_Kind::XOR,       // '^'

    Token_Kind::Identifier,// '_'

    Token_Kind::Error,

    // Lowercase letters
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier, Token_Kind::Identifier,
    Token_Kind::Identifier, Token_Kind::Identifier,


    Token_Kind::LCurly_Brace, // '{'
    Token_Kind::Pipe, // '|'
    Token_Kind::RCurly_Brace, // '}'
    Token_Kind::NOT, // '~'

    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error, Token_Kind::Error,
    Token_Kind::Error, Token_Kind::Error,
  };

  constexpr static int IntegerBits[256] = {

    // Error code
    -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
    -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
    -3,-3,

    // Digits
    0,1,2,3,4,5,6,7,8,9,

    // Error Code
    -3,-3,-3,-3,-3,-3,-3,

    // Uppercase values
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,

    // Error code
    -3,-3,-3,-3,-3,-3,

    // Lowercase values
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30, 31,32,33,34,35,

    // Error code
    -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
    -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
    -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
    -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
    -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
    -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
  };

  struct Reserved_Ident {
    const char *id;
    Token_Kind type;
  };

  static const struct Reserved_Ident Reserved[] {
    { "if", Keyword_If },
    { "else", Keyword_Else },
    { "elif", Keyword_Elif },
    { "while", Keyword_While },
    { "for", Keyword_For },
    { "true", Keyword_True },
    { "false", Keyword_False },
    { "None", Keyword_None },
    { "fn", Keyword_Function },
    { "struct", Keyword_Struct },
    { "export", Keyword_Export },
    { "enum", Keyword_Enum },
    { "extern", Keyword_Extern },
    { "error", Keyword_Error },
    { "var", Keyword_Var },
    { "return", Keyword_Return },
    { "and", Keyword_And },
    { "or", Keyword_Or },
    { "break", Keyword_Break },
    { "continue", Keyword_Continue },
    { "defer", Keyword_Defer },
    { "union", Keyword_Union },
    { "match", Keyword_Match },
    { "in", Keyword_In },
    {"str", Typename_str },
    {"i8", Typename_i8 },
    {"i16", Typename_i16 },
    {"i32", Typename_i32 },
    {"i64", Typename_i64 },
    {"u8", Typename_u8 },
    {"u16", Typename_u16 },
    {"u32", Typename_u32 },
    {"u64", Typename_u64 },
    {"box", Typename_box },
    {"f32", Typename_f32 },
    {"f64", Typename_f64 },
  };

  struct Lexer {
    const char    *buffer   = nullptr;
    const size_t  buf_size  = 0;
    size_t        line      = 1;
    size_t        column    = 1;
    size_t        buf_pos   = 0;
    Diagnostic    diagnostics;
    Range<Token *>tokens{};
  public:
    [[nodiscard]] NG_AINLINE auto peek(size_t chars) const { return *(buffer + chars); }
    [[nodiscard]] NG_AINLINE auto curr() const { return *buffer; }

    template<Lex_Mode Mode>
    auto next(size_t n) {
      if (buf_pos >= buf_size) {

        if constexpr (Mode is TokenMode) {
          diagnostics.build(Lex_Error {
            Lex_Error_Kind::Out_Of_Range,
            {line, column},
            Token("\0", {line, column}, Token_Kind::EOFToken)
          });
          UNREACHABLE;
        }

      }
      column += n; buf_pos += n; buffer += n;
      return *buffer;
    }
  };


  static Token_Kind MatchIdentToReserved(const char *ident, size_t len) {
    FOR(i, SizeOfArray(Reserved)) {
      if (std::strncmp(Reserved[i].id, ident, len) is 0)
        return Reserved[i].type;
    }
    return Identifier;
  }


  template<Lex_Mode Mode>
  static auto lex_float(Lexer &L, int skip, int start) {
    // Initialize start of token
    auto token_start = L.buffer-skip;
    auto token_start_col = L.column;

    L.next<Mode>(skip-1);

    auto decimal = static_cast<double>(0);
    auto divisor = static_cast<double>(1);
    auto value   = static_cast<double>(0);

    // TODO: This might be really slow ...
    do {
      auto digit = IntegerBits[L.curr()];
      if (digit < 0) break;
      divisor *= static_cast<double>(10);
      value = value * static_cast<double>(10) + static_cast<double>(digit) ;
    } while(NEXT);

    decimal = (value / divisor);

    auto token_end = L.buffer;
    auto literal_token_representation = std::string_view(token_start, token_end-token_start);

    if constexpr (Mode is PrintingMode) {
      Console::Log(Colors::BLUE, literal_token_representation, Colors::RESET);
    }

    L.tokens.end = new Token {
      literal_token_representation,
      {L.line, L.column},
      (static_cast<double>(start) + decimal),
      Token_Kind::Decimal,
      Token_Flags::Literal
    };
  }

  template<Lex_Mode Mode, int radix, int skip>
  static auto lex_int(Lexer &L) {


    // Initialize start of token
    const char *token_start = L.buffer;
    size_t token_start_col = L.column;

    // Skip any L.tokens, if any (for 0x, 0b, 00 cases)
    L.next<Mode>(skip);

    // Note: Keep the type!
    decltype(UINTPTR_MAX) lexed_int = 0;
    do {
      auto digit = IntegerBits[L.curr()];
      if (digit < 0) break;

      // Invalid digit in number. For example, in base 10,
      // we accept 1234 as a valid number but not 123f as 'f' is out of
      // range for the accepted base.
      if (digit >= radix) {
        if constexpr (Mode is TokenMode) {
          L.diagnostics.build(Lex_Error {
            .error = Digit_Out_Of_Range,
            .location = {
              .line   = L.line,
              .column = L.column,
              .source_span = {}
            },
            .failed_token = Token {
              std::string_view(token_start, L.buffer-token_start),
              {L.line, token_start_col},
              Token_Kind::Integer
            },
            .metadata = {radix, digit + 1}
          });
        }
      }
      // Check whether value is about to overflow the maximum integer value.
      // Ensures that all integers don't exceed 64-bit max.
      if (lexed_int > (UINTPTR_MAX - digit) / radix) {
        if constexpr (Mode is TokenMode) {
          L.diagnostics.build(Lex_Error {
            Lex_Error_Kind::Integer_Overflow,
            {L.line, L.column},
            Token {
              std::string_view(token_start, L.buffer-token_start),
              {L.line, token_start_col},
              Token_Kind::Integer
            }
          });
        }
      }
      lexed_int = lexed_int * radix + digit;
    } while(NEXT);


    if (L.curr() is '.')  { // At this point, we've realized its f64, so we proceed
      lex_float<Mode>(L, L.buffer - token_start, lexed_int);
      return;
    }

    const char *token_end = L.buffer;
    auto literal_token_representation = std::string_view(token_start, token_end-token_start);

    if constexpr (Mode is PrintingMode) {
      Console::Log(Colors::BLUE, (literal_token_representation), Colors::RESET);
    }


    L.tokens.end = new Token {
      literal_token_representation,
      {L.line, L.column},
      (decltype(UINTPTR_MAX)) lexed_int,
      Token_Kind::Integer,
      Token_Flags::Literal
    };
  }

  template<Lex_Mode Mode>
  static auto lex_int(Lexer &L) {
    switch (L.curr()) {
      case '0':
        switch(L.peek(1)) {
          case 'b': lex_int<Mode, 2, 2>(L);  return;
          case 'x': lex_int<Mode, 16, 2>(L); return;
          case '#': lex_int<Mode, 36, 2>(L); return;
          case '0': case '1': case '2': case '3': case '4': case '5':
          case '6': case '7': case '8': case '9': lex_int<Mode, 8, 1>(L); return;
          case '.': lex_float<Mode>(L, 2, 0); return;
          default: lex_int<Mode, 10, 0>(L); return;
        }
      default: lex_int<Mode, 10, 0>(L);
    }
  }


  template<Lex_Mode Mode>
  static auto lex_ident(Lexer &L) {
    const char *token_start = L.buffer;
    auto token_start_col = L.column;
    do {} while(MatchTokenKind[NEXT] is Token_Kind::Identifier);
    const char *token_end   = L.buffer;

    auto literal_token_representation = std::string_view(token_start, token_end-token_start);
    Token_Kind reserved = MatchIdentToReserved(token_start, token_end - token_start);

    if constexpr (Mode is PrintingMode) {
      if (reserved not_eq Token_Kind::Identifier) {
        Console::Log(Colors::RED, literal_token_representation, Colors::RESET);
      } else {
        Console::Log(Colors::YELLOW, literal_token_representation, Colors::RESET);
      }
    }

    L.tokens.end = new Token {
      literal_token_representation,
      {L.line, token_start_col},
      "",
      reserved,
      static_cast<unsigned int>(reserved is Identifier ? Keyword_ : -1)
    };
  }

  template<Lex_Mode Mode>
  static auto lex_str(Lexer &L) {
    auto token_start = L.buffer;
    auto token_start_col = L.column;
    auto offset = 1;

    if constexpr (Mode is PrintingMode) {
      Console::Log(Colors::GREEN);
    }

    // @NOTE: Remember that this is DATA not OBJECT for allocation
    auto lexed_string = (char*) (mem::GLOBAL_DATA_ALLOC.current() + 1);
    size_t len = 0;

    char current_char = NEXT;
    if constexpr (Mode is PrintingMode) {
      Console::Log("\"");
    }
    
    do {
      current_char = L.curr();
      if constexpr (Mode is PrintingMode) {
        Console::Log(current_char);
        if (current_char is '\0' or L.peek(1) is '\0') {
          offset = 0;
          break;
        }
      } else {
        if (current_char is '\0') {
          L.diagnostics.build(Lex_Error {
            Lex_Error_Kind::Missing_Closing_Delim,
            {L.line, L.column},
            Token {
              std::string_view(token_start, L.buffer-token_start),
              {L.line, token_start_col},
              Token_Kind::String
            },
            {(int)('"')}
          });
        }

        if (current_char is '\\') {
          switch (NEXT) {
            case 'a': current_char = '\a'; break;
            case 'b': current_char = '\b'; break;
            case 'r': current_char = '\r'; break;
            case 'n': current_char = '\n'; break;
            case 'v': current_char = '\v'; break;
            case 't': current_char = '\t'; break;
            case 'x': {
              auto value = IntegerBits[NEXT];
              if (value >= 16) {
                L.diagnostics.build(Lex_Error{
                  Lex_Error_Kind::Hex_Escape_Out_Of_Range,
                  {L.line, L.column},
                  Token {
                    std::string_view(token_start, L.buffer-token_start),
                    {L.line, token_start_col},
                    Token_Kind::String
                  },
                  {value + 1} // Pass in the invalid digit for the escape
                });
              }
              auto value2 = IntegerBits[NEXT];
              if (value2 >= 16) {
                L.diagnostics.build(Lex_Error{
                  Lex_Error_Kind::Hex_Escape_Out_Of_Range,
                  {L.line, L.column},
                  Token {
                    std::string_view(token_start, L.buffer-token_start),
                    {L.line, token_start_col},
                    Token_Kind::String
                  },
                  {value2 + 1} // Pass in the invalid digit for the escape
                });
              }
              int val = value;
              val = val * 16 + value2; // @Opt: (val << 4) | value2 ?

              current_char = (char) val;
              break;
            }
            default:
              L.diagnostics.build(Lex_Error {
                Lex_Error_Kind::Invalid_String_Escape,
                {L.line, L.column},
                Token {
                  std::string_view(token_start, L.buffer-token_start),
                  {L.line, token_start_col},
                  Token_Kind::String
                }
              });
          }
        }
        lexed_string[len++] = current_char;
      }
    } while(NEXT not_eq '"');

    NEXT; // Skip '"'

    // For missing closing expect_delim this needs to be here
    if (Mode is PrintingMode and offset != 0) Console::Log("\"");
    if constexpr (Mode is TokenMode) {
      lexed_string[len] = '\0';

      // We wrote to the data L.buffer without telling the size of the string
      // because we did not know what it was, so we tell it the size of the
      // string after lexing it. `reserve(size_t)` in this case is used to
      // increment the number of bytes written.
      mem::GLOBAL_DATA_ALLOC.reserve(len);
      L.tokens.end = new Token {
        std::string_view {lexed_string, len},
        {L.line, L.column},
        "",
        Token_Kind::String,
        Token_Flags::Literal
      };
    }
  }

  template<Lex_Mode Mode>
  static void skip_new_line(Lexer &L) {
    L.line++;
    L.column = 1;
    NEXT;

    char current = *L.buffer;
    char next_ch  = L.peek(1);

    if (next_ch != current and (next_ch is '\n' or next_ch is '\r')) {
      if constexpr (Mode is Lex_Mode::PrintingMode)
        Console::Log(next_ch);
      ++L.line, L.column = 1, NEXT;
    }
  }

  template<Lex_Mode Mode>
  auto lex(const char *buffer, const char *filename, size_t buffer_length) -> std::vector<Token> {
    auto L = Lexer {
      .buffer       = buffer,
      .buf_size     = buffer_length,
      .line         = 1,
      .column       = 1,
      .buf_pos      = 0,
      .diagnostics  = Diagnostic(buffer, buffer_length, filename),
      .tokens       = {}
    };

    L.tokens.begin = (Token *) mem::GLOBAL_OBJECT_ALLOC.current();

    Token_Kind kind;
    while(true) {
      //if (fatal) break; // Special case: Diagnostics may crash

      auto current_char = L.curr();
      kind = MatchTokenKind[current_char];
      if constexpr (Mode is PrintingMode) Console::Log(Colors::RESET);

      switch (kind) {
        case EOFToken: {
          L.tokens.end = new Token("a", {L.line, L.column}, Token_Kind::EOFToken);
          return std::vector<Token>(L.tokens.begin, (Token*) new char('a'));
        }
        case Integer: lex_int<Mode>(L); break;
        case String: lex_str<Mode>(L); break;
        case Identifier: lex_ident<Mode>(L); break;
        case NewLine: skip_new_line<Mode>(L); break;
        case Whitespace: {
          if constexpr (Mode is PrintingMode)
            Console::Log(current_char);
          NEXT;
          break;
        }
        case Char: {
          auto token_start = L.buffer;
          auto token_start_col = L.column;
          if constexpr (Mode is PrintingMode) {
            Console::Log(Colors::GREEN, "'");
            Console::Log(NEXT, Colors::RESET);
          }
          else {
            auto ch = NEXT;
            L.tokens.end = new Token {
              std::string_view(&ch, 1),
              {L.line, token_start_col},
              ch,
              Token_Kind::Char,
            };
          }
          if (L.curr() not_eq '\'') {
            if constexpr (Mode is TokenMode) {
              L.diagnostics.build(Lex_Error {
                Lex_Error_Kind::Missing_Closing_Delim,
                {L.line, L.column}, // 'x[?] <-- Missing Closing!
                Token {
                  std::string_view(token_start, (L.buffer+1)-token_start),
                  {L.line, token_start_col},
                  Token_Kind::String
                },
                {(char)'\''}
              });
            }
            NEXT; // Skip "'"
            break;
          }
          else if constexpr (Mode is PrintingMode) Console::Log("'", Colors::RESET);
          NEXT; // Skip the "'"
          break;
        }
        case Less_Than: { // <, <=, <<, <<=
          auto n = L.peek(1);
          if (n is '<') {
            if (L.peek(2) is '=') {
              TOKEN_ADD_SHORT("<<=", Token_Kind::Left_Shift_Equals);
              break;
            }
            TOKEN_ADD_SHORT("<<", Token_Kind::Left_Shift);
            break;
          }
          else if (n is '=') {
            TOKEN_ADD_SHORT("<=", Token_Kind::Less_Than_Equals);
            break;
          }
          TOKEN_ADD_SHORT("<", Token_Kind::Less_Than);
          break;
        }
        case Greater_Than: {
          auto n = L.peek(1);
          if (n is '>') {
            if (L.peek(2) is '=') {
              TOKEN_ADD_SHORT(">>=", Token_Kind::Right_Shift_Equals);
              break;
            }
            TOKEN_ADD_SHORT(">>", Token_Kind::Right_Shift);
            break;
          }
          else if (n is '=') {
            TOKEN_ADD_SHORT(">=", Token_Kind::Greater_Than_Equals);
            break;
          }
          TOKEN_ADD_SHORT(">", Token_Kind::Greater_Than);
          break;
        }
        case Star: {
          auto n = L.peek(1);
          if (n is '*') {
            if (L.peek(2) is '=') {
              TOKEN_ADD_SHORT("**=", Token_Kind::Pow_Equals);
              break;
            }
            TOKEN_ADD_SHORT("**", Token_Kind::Pow);
            break;
          }
          else if (n is '=') {
            TOKEN_ADD_SHORT("*=", Token_Kind::Mul_Equals);
            break;
          }
          TOKEN_ADD_SHORT("*", Token_Kind::Star);
          break;
        }
        case Dot: {
          auto n = L.peek(1);
          if (n is '.') {
            if (L.peek(2) is '.') {
              TOKEN_ADD_SHORT("...", Token_Kind::Ellipsis);
              break;
            }
            TOKEN_ADD_SHORT("..", Token_Kind::Range_Span);
            break;
          }
          TOKEN_ADD_SHORT(".", Token_Kind::Dot);
          break;
        }
        case Plus: {
          auto n = L.peek(1);
          if (n is '+') { TOKEN_ADD_SHORT("++", Token_Kind::Plus_Plus);}
          else if (n is '=')  { TOKEN_ADD_SHORT("+=", Token_Kind::Plus_Equals); }
          else {
            TOKEN_ADD_SHORT("+", Token_Kind::Plus);
          }
          break;
        }
        case Minus: {
          auto n = L.peek(1);
          if (n is '-') {
            TOKEN_ADD_SHORT("--", Token_Kind::Minus_Minus);
          }
          else if (n is '=') {
            TOKEN_ADD_SHORT("-=", Token_Kind::Minus_Minus);
          }
          else if (n is '>') {
            TOKEN_ADD_SHORT("->", Token_Kind::Arrow);
          }
          else {
            TOKEN_ADD_SHORT("+", Token_Kind::Minus);
          }
          break;
        }
        case Slash: {
          auto n = L.peek(1);
          if (n is '=') {
            TOKEN_ADD_SHORT("/=", Token_Kind::Div_Equals);
          }
          else if (n is '/') {
            if constexpr (Mode is PrintingMode) Console::Log("/");
            while (MatchTokenKind[NEXT] != NewLine) {
              if constexpr (Mode is PrintingMode) {
                Console::Log(L.curr());
              }
            }
            skip_new_line<Mode>(L);
          }
          else {
            TOKEN_ADD_SHORT("/", Token_Kind::Slash);
          }
          break;
        }
        case Equals: {
          auto n = L.peek(1);
          if (n is '=') {
            TOKEN_ADD_SHORT("is", Token_Kind::Equals_Equals);
          } else if (n is '>') {
            TOKEN_ADD_SHORT("=>", Token_Kind::Function_Arrow);
          } else {
            TOKEN_ADD_SHORT("=", Token_Kind::Equals);
          }
          break;
        }
        case Percent: {
          auto n = L.peek(1);
          if (n is '=') {
            TOKEN_ADD_SHORT("%=", Token_Kind::Percent_Equals);
          } else {
            TOKEN_ADD_SHORT("%", Token_Kind::Percent);
          }
          break;
        }
        case AND: {
          auto n = L.peek(1);
          if (n is '=') {
            TOKEN_ADD_SHORT("&=", Token_Kind::AND);
          } else {
            TOKEN_ADD_SHORT("&", Token_Kind::AND_Equals);
          }
          break;
        }
        case Pipe: {
          auto n = L.peek(1);
          if (n is '=') {
            TOKEN_ADD_SHORT("|=", Token_Kind::OR_Equals);
          } else {
            TOKEN_ADD_SHORT("|", Token_Kind::Pipe);
          }
          break;
        }
        case XOR: {
          auto n = L.peek(1);
          if (n is '=') {
            TOKEN_ADD_SHORT("^=", Token_Kind::XOR_Equals);
          } else {
            TOKEN_ADD_SHORT("^", Token_Kind::XOR);
          }
          break;
        }
        case LParenthesis:
          TOKEN_ADD_SHORT("(", Token_Kind::LParenthesis);
          break;
        case RParenthesis:
          TOKEN_ADD_SHORT(")", Token_Kind::RParenthesis);
          break;
        case LCurly_Brace:
          TOKEN_ADD_SHORT("{", Token_Kind::LCurly_Brace);
          break;
        case RCurly_Brace:
          TOKEN_ADD_SHORT("}", Token_Kind::RCurly_Brace);
          break;
        case LBracket:
          TOKEN_ADD_SHORT("[", Token_Kind::LBracket);
          break;
        case RBracket:
          TOKEN_ADD_SHORT("]", Token_Kind::RBracket);
          break;
        case Semi_Colon:
          TOKEN_ADD_SHORT(";", Token_Kind::Semi_Colon);
          break;
        case At:
          TOKEN_ADD_SHORT("@", Token_Kind::At);
          break;
        case NOT:
          TOKEN_ADD_SHORT("~", Token_Kind::NOT);
          break;
        case Exclamation_Point:
          TOKEN_ADD_SHORT("!", Token_Kind::Exclamation_Point);
          break;
        case Question_Mark:
          TOKEN_ADD_SHORT("?", Token_Kind::Question_Mark);
          break;
        case Colon:
          if (L.peek(1) is '=')
            TOKEN_ADD_SHORT(":=", Token_Kind::Colon_Equals);
          else if (L.peek(1) is ':')
            TOKEN_ADD_SHORT("::", Token_Kind::Path);
          else
          TOKEN_ADD_SHORT(":", Token_Kind::Colon);
          break;
        case Comma:
          TOKEN_ADD_SHORT(",", Token_Kind::Comma);
          break;
        default:
          break;
      }
    }
    return std::vector<Token>(L.tokens.begin, (Token*) new char('a'));
  }


} // namespace nextgen::jet



#endif //NEXTGEN_LEXER_H
