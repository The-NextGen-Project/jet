#include "lexer.h"

using namespace nextgen;
using namespace nextgen::jet;

#define TOKEN_SHORT(value, kind) \
new Token { \
  value, \
  {line, column}, \
  value, \
  kind \
}
#define TOKEN_ADD_SHORT(value, kind) \
if constexpr (OUTPUT_MODE)           \
    Console::Log(value), next((sizeof value) - 1); \
else                                 \
  (tokens.end = TOKEN_SHORT(value, kind), next((sizeof value) - 1)) \
\

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

static const Token_Kind
MatchIdentToReserved(const char *ident, size_t len) {
  FOR(i, SizeOfArray(Reserved)) {
    if (::strncmp(Reserved[i].id, ident, len) == 0)
      return Reserved[i].type;
  }
  return Identifier;
}


template<Lex_Mode Mode> void
Lexer<Mode>::lex_float(int skip, int start) {
  // Initialize start of token
  auto token_start = buffer-skip;
  auto token_start_col = column;

  next(skip-1);

  double decimal = 0;
  double divisor = 1;
  double value   = 0;

  // TODO: This might be really slow ...
  do {
    auto digit = IntegerBits[curr()];
    if (digit < 0) break;
    divisor *= 10;
    value = value * 10 + digit ;
  } while(next(1));

  decimal = (value / divisor);

  auto token_end = buffer;
  auto literal_token_representation = std::string_view(token_start, token_end);

  if constexpr (OUTPUT_MODE) {
    Console::Log(Colors::BLUE, literal_token_representation, Colors::RESET);
  }

  tokens.end = new Token {
    literal_token_representation,
    {line, column},
    (double)(start + decimal),
    Token_Kind::Decimal,
    Token_Flags::Literal
  };
}

template<Lex_Mode Mode> void 
Lexer<Mode>::lex_int() {
  switch (this->curr()) {
    case '0':
      switch(peek(1)) {
        case 'b': this->lex_int<2, 2>();  return;
        case 'x': this->lex_int<16, 2>(); return;
        case '#': this->lex_int<36, 2>(); return;
        case '0': case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': this->lex_int<8, 1>(); return;
        case '.': this->lex_float(2); return;
        default: this->lex_int<10, 0>(); return;
      }
      default: this->lex_int<10, 0>();
  }
}

template<Lex_Mode Mode>
template<int radix, int skip> void
Lexer<Mode>::lex_int() {


  // Initialize start of token
  const char *token_start = buffer;
  size_t token_start_col = column;

  // Skip any tokens, if any (for 0x, 0b, 00 cases)
  next(skip);

  // Note: Keep the type!
  decltype(UINTPTR_MAX) lexed_int = 0;
  do {
    auto digit = IntegerBits[curr()];
    if (digit < 0) break;

    // Invalid digit in number. For example, in base 10,
    // we accept 1234 as a valid number but not 123f as 'f' is out of
    // range for the accepted base.
    if (digit >= radix) {
      if constexpr (NORMAL_MODE) {
        diagnostics.build(Lex_Error {
          Lex_Error_Kind::Digit_Out_Of_Range,
          {line, column},
          Token {
            std::string_view(token_start, buffer),
            {line, token_start_col},
            Token_Kind::Integer
            },
          {radix, digit + 1}
        });
      }
    }
    // Check whether value is about to overflow the maximum integer value.
    // Ensures that all integers don't exceed 64-bit max.
    if (lexed_int > (UINTPTR_MAX - digit) / radix) {
      if constexpr (NORMAL_MODE) {
        diagnostics.build(Lex_Error {
          Lex_Error_Kind::Integer_Overflow,
          {line, column},
          Token {
            std::string_view(token_start, buffer),
            {line, token_start_col},
            Token_Kind::Integer
          }
        });
      }
    }
    lexed_int = lexed_int * radix + digit;
  } while(next(1));


  if (curr() == '.')  { // At this point, we've realized its f64, so we proceed
    lex_float(buffer - token_start, lexed_int);
    return;
  }

  const char *token_end = buffer;
  auto literal_token_representation
          = std::string_view(token_start, token_end);

  if constexpr (OUTPUT_MODE) {
    Console::Log(Colors::BLUE, (literal_token_representation), Colors::RESET);
  }


  tokens.end = new Token {
    literal_token_representation,
    {line, column},
    (decltype(UINTPTR_MAX)) lexed_int,
    Token_Kind::Integer,
    Token_Flags::Literal
  };
}

template<Lex_Mode Mode>
void Lexer<Mode>::lex_ident() {
  const char *token_start = buffer;
  auto token_start_col = column;
  do {} while(MatchTokenKind[next(1)] == Token_Kind::Identifier);
  const char *token_end   = buffer;

  auto literal_token_representation = std::string_view(token_start, token_end);
  Token_Kind reserved = MatchIdentToReserved(token_start, token_end - token_start);

  if constexpr (OUTPUT_MODE) {
    if (reserved not_eq Token_Kind::Identifier) {
      Console::Log(Colors::RED, literal_token_representation,
                   Colors::RESET);
    } else {
      Console::Log(Colors::YELLOW, literal_token_representation, Colors::RESET);
    }
  }

  tokens.end = new Token {
    literal_token_representation,
    {line, token_start_col},
    "",
    reserved,
    static_cast<unsigned int>(reserved == Identifier ? Keyword_ : -1)
  };
}

template<Lex_Mode Mode>
void Lexer<Mode>::lex_str() {
  auto token_start = buffer;
  auto token_start_col = column;
  auto offset = 1;

  if constexpr (OUTPUT_MODE) Console::Log(Colors::GREEN);

  // @NOTE: Remember that this is DATA not OBJECT for allocation
  auto lexed_string = (char*) (mem::GLOBAL_DATA_ALLOC.current() + 1);
  size_t len = 0;

  char current_char = next(1);
  if constexpr (OUTPUT_MODE) Console::Log("\"");
  do {
    current_char = curr();
    if constexpr (OUTPUT_MODE) {
      Console::Log(current_char);
      if (current_char == '\0' or peek(1) == '\0') {
        offset = 0;
        break;
      }
    } else {
      if (current_char == '\0') {
        diagnostics.build(Lex_Error {
          Lex_Error_Kind::Missing_Closing_Delim,
          {line, column},
          Token {
            std::string_view(token_start, buffer),
            {line, token_start_col},
            Token_Kind::String
            },
          {(int)('"'
            )}
          });
      }

      if (current_char == '\\') {
        switch (next(1)) {
          case 'a': current_char = '\a'; break;
          case 'b': current_char = '\b'; break;
          case 'r': current_char = '\r'; break;
          case 'n': current_char = '\n'; break;
          case 'v': current_char = '\v'; break;
          case 't': current_char = '\t'; break;
          case 'x': {
            auto value = IntegerBits[next(1)];
            if (value >= 16) {
              diagnostics.build(Lex_Error{
                Lex_Error_Kind::Hex_Escape_Out_Of_Range,
                {line, column},
                Token {
                  std::string_view(token_start, buffer),
                  {line, token_start_col},
                  Token_Kind::String
                },
                {value + 1} // Pass in the invalid digit for the escape
              });
            }
            auto value2 = IntegerBits[next(1)];
            if (value2 >= 16) {
              diagnostics.build(Lex_Error{
                Lex_Error_Kind::Hex_Escape_Out_Of_Range,
                {line, column},
                Token {
                  std::string_view(token_start, buffer),
                  {line, token_start_col},
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
            diagnostics.build(Lex_Error {
              Lex_Error_Kind::Invalid_String_Escape,
              {line, column},
              Token {
                std::string_view(token_start, buffer),
                {line, token_start_col},
                Token_Kind::String
              }
            });
        }
      }
      lexed_string[len++] = current_char;
    }
  } while(next(1) not_eq '"');

  next(1); // Skip '"'

  // For missing closing expect_delim this needs to be here
  if (OUTPUT_MODE and offset != 0) Console::Log("\"");
  if constexpr (NORMAL_MODE) {
    lexed_string[len] = '\0';

    // We wrote to the data buffer without telling the size of the string
    // because we did not know what it was, so we tell it the size of the
    // string after lexing it. `reserve(size_t)` in this case is used to
    // increment the number of bytes written.
    mem::GLOBAL_DATA_ALLOC.reserve(len);
    tokens.end = new Token {
      std::string_view {lexed_string, len},
      {line, column},
      "",
      Token_Kind::String,
      Token_Flags::Literal
    };
  }
}

template<Lex_Mode Mode> std::vector<Token>
Lexer<Mode>::lex() {
  tokens.begin = (Token *) mem::GLOBAL_OBJECT_ALLOC.current();

  Token_Kind kind;
  while(true) {
    if (fatal) break; // Special case: Diagnostics may crash

    auto current_char = curr();
    kind = MatchTokenKind[current_char];
    if constexpr (OUTPUT_MODE) Console::Log(Colors::RESET);

    switch (kind) {
      case EOFToken: {
        tokens.end = new Token("a", {line, column}, Token_Kind::EOFToken);
        return std::vector<Token>(tokens.begin, (Token*) new char('a'));
      }
      case Integer: lex_int(); break;
      case String: lex_str(); break;
      case Identifier: lex_ident(); break;
      case NewLine: skip_new_line(); break;
      case Whitespace: {
        if constexpr (OUTPUT_MODE)
          Console::Log(current_char);
        next(1);
        break;
      }
      case Char: {
        auto token_start = buffer;
        auto token_start_col = column;
        if constexpr (OUTPUT_MODE) {
          Console::Log(Colors::GREEN, "'");
          Console::Log(next(1), Colors::RESET);
        }
        else {
          auto ch = next(1);
          tokens.end = new Token {
            std::string_view(&ch, 1),
            {line, token_start_col},
            ch,
            Token_Kind::Char,
          };
        }
        if (curr() not_eq '\'') {
          if constexpr (NORMAL_MODE) {
            diagnostics.build(Lex_Error {
              Lex_Error_Kind::Missing_Closing_Delim,
              {line, column}, // 'x[?] <-- Missing Closing!
              Token {
                std::string_view(token_start, buffer+1),
                {line, token_start_col},
                Token_Kind::String
                },
              {(char)'\''}
            });
          }
          next(1); // Skip "'"
          break;
        }
        else if constexpr (OUTPUT_MODE) Console::Log("'", Colors::RESET);
        next(1); // Skip the "'"
        break;
      }
        case Less_Than: { // <, <=, <<, <<=
          auto n = peek(1);
          if (n == '<') {
            if (peek(2) == '=') {
              TOKEN_ADD_SHORT("<<=", Token_Kind::Left_Shift_Equals);
              break;
            }
            TOKEN_ADD_SHORT("<<", Token_Kind::Left_Shift);
            break;
          }
          else if (n == '=') {
            TOKEN_ADD_SHORT("<=", Token_Kind::Less_Than_Equals);
            break;
          }
          TOKEN_ADD_SHORT("<", Token_Kind::Less_Than);
          break;
        }
        case Greater_Than: {
          auto n = peek(1);
          if (n == '>') {
            if (peek(2) == '=') {
              TOKEN_ADD_SHORT(">>=", Token_Kind::Right_Shift_Equals);
              break;
            }
            TOKEN_ADD_SHORT(">>", Token_Kind::Right_Shift);
            break;
          }
          else if (n == '=') {
            TOKEN_ADD_SHORT(">=", Token_Kind::Greater_Than_Equals);
            break;
          }
          TOKEN_ADD_SHORT(">", Token_Kind::Greater_Than);
          break;
        }
        case Star: {
          auto n = peek(1);
          if (n == '*') {
            if (peek(2) == '=') {
              TOKEN_ADD_SHORT("**=", Token_Kind::Pow_Equals);
              break;
            }
            TOKEN_ADD_SHORT("**", Token_Kind::Pow);
            break;
          }
          else if (n == '=') {
            TOKEN_ADD_SHORT("*=", Token_Kind::Mul_Equals);
            break;
          }
          TOKEN_ADD_SHORT("*", Token_Kind::Star);
          break;
        }
        case Dot: {
          auto n = peek(1);
          if (n == '.') {
            if (peek(2) == '.') {
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
          auto n = peek(1);
          if (n == '+') { TOKEN_ADD_SHORT("++", Token_Kind::Plus_Plus);}
          else if (n == '=')  { TOKEN_ADD_SHORT("+=", Token_Kind::Plus_Equals); }
          else {
            TOKEN_ADD_SHORT("+", Token_Kind::Plus);
          }
          break;
        }
        case Minus: {
          auto n = peek(1);
          if (n == '-') {
            TOKEN_ADD_SHORT("--", Token_Kind::Minus_Minus);
          }
          else if (n == '=') {
            TOKEN_ADD_SHORT("-=", Token_Kind::Minus_Minus);
          }
          else if (n == '>') {
            TOKEN_ADD_SHORT("->", Token_Kind::Arrow);
          }
          else {
            TOKEN_ADD_SHORT("+", Token_Kind::Minus);
          }
          break;
        }
        case Slash: {
          auto n = peek(1);
          if (n == '=') {
            TOKEN_ADD_SHORT("/=", Token_Kind::Div_Equals);
          }
          else if (n == '/') {
            if constexpr (OUTPUT_MODE) Console::Log("/");
            while (MatchTokenKind[next(1)] != NewLine) {
              if constexpr (OUTPUT_MODE) {
                Console::Log(curr());
              }
            }
            skip_new_line();
          }
          else {
            TOKEN_ADD_SHORT("/", Token_Kind::Slash);
          }
          break;
        }
        case Equals: {
          auto n = peek(1);
          if (n == '=') {
            TOKEN_ADD_SHORT("==", Token_Kind::Equals_Equals);
          } else if (n == '>') {
            TOKEN_ADD_SHORT("=>", Token_Kind::Function_Arrow);
          } else {
            TOKEN_ADD_SHORT("=", Token_Kind::Equals);
          }
          break;
        }
        case Percent: {
          auto n = peek(1);
          if (n == '=') {
            TOKEN_ADD_SHORT("%=", Token_Kind::Percent_Equals);
          } else {
            TOKEN_ADD_SHORT("%", Token_Kind::Percent);
          }
          break;
        }
        case AND: {
          auto n = peek(1);
          if (n == '=') {
            TOKEN_ADD_SHORT("&=", Token_Kind::AND);
          } else {
            TOKEN_ADD_SHORT("&", Token_Kind::AND_Equals);
          }
          break;
        }
        case Pipe: {
          auto n = peek(1);
          if (n == '=') {
            TOKEN_ADD_SHORT("|=", Token_Kind::OR_Equals);
          } else {
            TOKEN_ADD_SHORT("|", Token_Kind::Pipe);
          }
          break;
        }
        case XOR: {
          auto n = peek(1);
          if (n == '=') {
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
          if (peek(1) == '=')
            TOKEN_ADD_SHORT(":=", Token_Kind::Colon_Equals);
          else if (peek(1) == ':')
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
  return std::vector<Token>(tokens.begin, (Token*) new char('a'));
}

// Forces compilation of the two lexer classes
// so that we can have definitions in a cpp file without
// crowding the header file.
namespace nextgen { namespace jet {
  template class Lexer<TokenMode>;
  template class Lexer<PrintingMode>;
}}