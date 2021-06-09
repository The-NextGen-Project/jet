# include <nextgen/jet/lex/lexer.h>

using namespace nextgen;
using namespace nextgen::jet;

# define TOKEN(type, text, k, value) Token::New ( \
    text, \
    Token::TokenValue { .kind = k }, \
  {this->line, this->col},                      \
  value                                              \
)



static constexpr TokenKind TokenKindClass[256] {

  // Error code
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Whitespace, // \a
  TokenKind::Whitespace, // \b
  TokenKind::Whitespace, // <space>
  TokenKind::NewLine,    // \n
  TokenKind::Whitespace, // \v
  TokenKind::Error,
  TokenKind::NewLine,    // \r

  TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error,


  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,

  TokenKind::Whitespace,    // \t

  TokenKind::Error,
  TokenKind::String,        // '"'
  TokenKind::Error, TokenKind::Error,
  TokenKind::Percent,       // '%'
  TokenKind::AND,           // '&'
  TokenKind::Char,          // '\''
  TokenKind::LParenthesis,  // (
  TokenKind::RParenthesis,  // )
  TokenKind::Star,          // '*'
  TokenKind::Plus,          // '+'
  TokenKind::Error,

  TokenKind::Minus, // '-'


  TokenKind::Dot, // '.'

  TokenKind::Slash, // '/'


  // Digits
  TokenKind::Integer, TokenKind::Integer, TokenKind::Integer,
  TokenKind::Integer, TokenKind::Integer,
  TokenKind::Integer, TokenKind::Integer, TokenKind::Integer,
  TokenKind::Integer, TokenKind::Integer,


  TokenKind::Colon, // ':'

  // Error Code
  TokenKind::Error,
  TokenKind::LessThan, // '<'
  TokenKind::Equals, // '='
  TokenKind::GreaterThan, // '>'
  TokenKind::QuestionMark, // '?'
  TokenKind::Error,

  // Uppercase letters
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier,


  TokenKind::LBracket,  // '['

  TokenKind::StringEscapeStart, // \\

  TokenKind::RBracket,  // ']'

  TokenKind::XOR,       // '^'

  TokenKind::Identifier,// '_'

  TokenKind::Error,

  // Lowercase letters
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
  TokenKind::Identifier, TokenKind::Identifier,


  TokenKind::LCurlyBrace, // '{'
  TokenKind::Pipe, // '|'
  TokenKind::RCurlyBrace, // '}'
  TokenKind::NOT, // '~'

  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error,
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


struct JetKeyword {
  str    key;
  TokenKind id;
};

static const struct JetKeyword Keywords[] {
  { "if"_intern, KeywordIf },
  { "else"_intern, KeywordElse },
  { "elif"_intern, KeywordElif },
  { "while"_intern, KeywordWhile },
  { "for"_intern, KeywordFor },
  { "true"_intern, KeywordTrue },
  { "false"_intern, KeywordFalse },
  { "None"_intern, KeywordNone },
  { "fn"_intern, KeywordFunction },
  { "struct"_intern, KeywordStruct },
  { "export"_intern, KeywordExport },
  { "enum"_intern, KeywordEnum },
  { "extern"_intern, KeywordExtern },
  { "error"_intern, KeywordError },
  { "var"_intern, KeywordVar },
  { "return"_intern, KeywordReturn },
  { "and"_intern, KeywordAnd },
  { "or"_intern, KeywordOr },
  { "break"_intern, KeywordBreak },
  { "continue"_intern, KeywordContinue },
  { "defer"_intern, KeywordDefer },
  { "union"_intern, KeywordUnion },
};

// Determine the validity of parsed string being a Keyword. The assumption
// made in this function is that `value` is an interned string therefore only
// direct pointer comparison is made resulting in O(1) comparison.
static auto ReturnValidKeyword(str &value, TokenKind else_) -> TokenKind {
  for (auto i = 0; i < SizeOfArray(Keywords); ++i) {
    if (value == Keywords[i].key)
      return Keywords[i].id;
  }
  return else_;
}




auto Lexer::NextToken() -> Result<Token, LexError> {
  auto kind     = TokenKindClass[Curr()];
  auto token    = Token {};

  goto start;
  start:
  {
    switch (kind) {

      case Integer: {

        // Integer Base
        int radix = 10;

        // FNV-1a Hash Value
        auto val  = FNV_OFF;

        // Token Suffix marker
        TokenClassification flags = static_cast<TokenClassification>(0);

        // Integer Allocator (25 chars is max integer length)
        Allocator *integer_allocator =
          allocator->GetAllocatorForAvailableSpace(25);

        // Integer Token ID
        char *id = (char *) integer_allocator->BlockPoint();

        // Token Len
        size_t len = 0;


        switch (Curr()) {
          case '0':
            switch (Next(1)) {
              case 'b':
                radix = 2;

                // Unroll-Hash For Skip
                val ^= '0';
                val *= FNV_PRIME;
                val ^= 'b';
                val *= FNV_PRIME;

                id[len++] = '0';
                id[len++] = 'b';

                Next(1);
                break;
              case 'x':
                radix = 16;

                // Unroll-Hash For Skip
                val ^= '0';
                val *= FNV_PRIME;
                val ^= 'x';
                val *= FNV_PRIME;

                id[len++] = '0';
                id[len++] = 'x';

                Next(1);
                break;
              case '#':
                radix = 36;

                // Unroll-Hash For Skip
                val ^= '0';
                val *= FNV_PRIME;
                val ^= '#';
                val *= FNV_PRIME;

                id[len++] = '0';
                id[len++] = '#';

                Next(1);
                break;
              case '0': case '1': case '2': case '3': case '4': case '5':
              case '6': case '7': case '8': case '9':
                radix = 8;

                // Unroll-Hash For Skip
                val ^= '0';
                val *= FNV_PRIME;

                id[len++] = '0';
                break;
              default: // TODO: Figure what needs to be done here
                break;
            }
            break;
          default:
            break;
        }


        decltype(UINTPTR_MAX) value = 0;
        do {

          auto curr = Curr();

          id[len++] = curr;

          // Calculate Identifier FNV-1a Hash
          val ^= curr;
          val *= FNV_PRIME;

          int digit = IntegerBits[curr];

          if (digit < 0) {
            break;
          }

          if (digit >= radix) {
            return Err<Token, LexError>( LexError {
              ErrorType::DigitOutOfRange,
              { this->line, this->col }
            });
          }
          if (value > (UINTPTR_MAX - digit) / radix) {
            return Err<Token, LexError>( LexError {
              ErrorType::IntegerOverflow,
              { this->line, this->col }
            });
          }
          value = value * radix + digit;
        } while (Next(1));

        // Token Suffix
        switch (Curr()) { // TODO: Add Implementation
          case 'p': // Power
            break;
          case 'u': // Unsigned
            flags = TokenClassification::Unsigned;
            break;
          case 'e': // x10^power
            break;
        }

        id[len++] = '\0';

        auto s = str(id, len);
        s.setHash(val);
        auto intern = StringInterner::Intern(s);

        token = TOKEN(decltype(UINTPTR_MAX), intern, TokenKind::Integer, value);
        token.setFlag(flags);
        return Ok<Token, LexError>(token);
      }
      case String:
        break;
      case Decimal:
        break;
      case Identifier: { // Maybe : Identifier length no more 256 characters?


        auto begin = this->file;

        // FNV-1a Hash on String as Lexing as to not do it twice
        // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_function
        auto val = FNV_OFF;
        do {

          val ^= Curr();
          val *= FNV_PRIME;
        } while (TokenKindClass[Next(1)] == TokenKind::Identifier);


        auto end = this->file;

        auto range = std::string(begin, end); // TODO: Fix Me ASAP
        auto s = str(range);
        s.setHash(val);
        auto intern = StringInterner::Intern(s);

        // Ensure appropriate kind is assigned
        auto type = ReturnValidKeyword(intern,TokenKind::Identifier);
        return Ok<Token, LexError>(TOKEN(const char *, intern, type, ""));
      }
      case LessThan: {
        auto s1 = TokenKindClass[Peek(1)];
        match (kind, s1) {
          set(TokenKind::LessThan, TokenKind::Equals):
            token = TOKEN(const char *, "<=", TokenKind::LessThan, "<=");
            token.assignment = true;
            break;
          set(TokenKind::LessThan, TokenKind::LessThan):
            kind = TokenKind::LeftShift;
            goto start;
          default:
            token = TOKEN(char, '<', TokenKind::LessThan, '<');
            break;
        }
        break;
      }
      case LeftShift: {
        auto s1 = TokenKindClass[Peek(1)];
        match (kind, s1) {
          set(TokenKind::LeftShift, TokenKind::Equals):
            token.assignment = true;
            token = TOKEN(const char *, "<<=", TokenKind::LeftShift, "<<=");
          default:
            token = TOKEN(const char *, "<<", TokenKind::LeftShift, "<<");
            break;
        }
        break;
      }
      case RightShift: {
        auto s1 = TokenKindClass[Peek(1)];
        match (kind, s1) {
          set(TokenKind::RightShift, TokenKind::Equals):
            token.assignment = true;
            token = TOKEN(const char *, ">>=", TokenKind::RightShift, ">>=");
          default:
            token = TOKEN(const char *, ">>", TokenKind::RightShift, ">>");
            break;
        }
        break;
      }
      case GreaterThan: {
        auto s1 = TokenKindClass[Peek(1)];
        match (kind, s1) {
          set(TokenKind::GreaterThan, TokenKind::Equals):
            token = TOKEN(const char *, ">=", TokenKind::GreaterThan, ">=");
            token.assignment = true;
            break;
          set(TokenKind::GreaterThan, TokenKind::GreaterThan):
            kind = TokenKind::LeftShift;
            goto start;
          default:
            token = TOKEN(char, '>', TokenKind::GreaterThan, '>');
            break;
        }
        break;
      }
      case Equals: {
        auto s1 = TokenKindClass[Peek(1)];
        if (s1 == TokenKind::Equals) {
          token = TOKEN(const char *, "==", TokenKind::EqualsEquals, "==");
        } else {
          token = TOKEN(char, '=', TokenKind::Equals, '=');
        }
        break;
      }
      case QuestionMark: {
        auto s1 = TokenKindClass[Peek(1)];
        if (s1 == TokenKind::QuestionMark) {
          token = TOKEN(const char *, "??", TokenKind::Then, "??");
        } else {
          token = TOKEN(char, '?', TokenKind::QuestionMark, '?');
        }
        break;
      }
      case Plus: {
        auto s1 = TokenKindClass[Peek(1)];
        match (kind, s1) {
          set(TokenKind::Plus, TokenKind::PlusPlus):
            token = TOKEN(const char *, "++", TokenKind::PlusPlus, "++");
            break;
          set(TokenKind::Plus, TokenKind::Equals):
            token = TOKEN(const char *, "+=", TokenKind::Plus, "+=");
            token.assignment = true;
          default:
            token = TOKEN(char, '+', TokenKind::Plus, '+');
            break;
        }
        break;
      }
      case Minus: {
        auto s1 = TokenKindClass[Peek(1)];
        match (kind, s1) {
          set(TokenKind::Minus, TokenKind::Minus):
            token = TOKEN(const char *, "--", TokenKind::MinusMinus, "--");
            break;
          set(TokenKind::Minus, TokenKind::Equals):
            token = TOKEN(const char *, "-=", TokenKind::MinusMinus, "-=");
            token.assignment = true;
          default:
            token = TOKEN(char, '-', TokenKind::Minus, '-');
            break;
        }
        break;
      }
      case Slash:
        break;
      case Star:
        break;
      case NewLine: // \n or \r
        SkipNewLine();
        goto start; // Does this so 2 function calls aren't wasted.
      case Char:
        token = TOKEN(char, Next(1), TokenKind::Char, Curr());
        if (TokenKindClass[Next(1)] != TokenKind::Char) {
          return Err<Token, LexError>(LexError{
            ErrorType::MissingClosingForChar,
            {this->line, this->col} // 'x[?] <-- Missing Closing!
          });
        }
        break;
      case Whitespace: // Skip whitespace
        Next(1);
        kind = TokenKindClass[Curr()];
        goto start;
      case Dot: {
        auto s1 = TokenKindClass[Peek(1)];
        auto s2 = TokenKindClass[Peek(2)];
        auto s3 = TokenKindClass[Peek(3)];

        match (kind, s1) {
          set(TokenKind::Dot, TokenKind::Dot):
            token = s2 == TokenKind::Dot ?
                    (TOKEN(const char *, "...",
                           TokenKind::Ellipsis, "...")) :
                    (TOKEN(const char *, "..",
                           TokenKind::RangeSpan, ".."));
            break;
          default:
            token = TOKEN(char, '.', TokenKind::Dot, '.');
            break;
        }
        // Cannot match token "...[.]" <-- Invalid Dot Here!
        if (s3 == TokenKind::Dot && token.getKind() == TokenKind::Ellipsis) {
          return Err<Token, LexError>(LexError{
            ErrorType::InvalidDot,
            {this->line, this->col + 1}
          });
        }

        break;
      }
        // Cannot be generated by the TokenKind table, assigned after checks
      case EqualsEquals:
      case Pow:
      case Boolean:
        UNREACHABLE;
        break;
      case Error:
        return Err<Token, LexError>(LexError{
          ErrorType::InvalidChar,
          {this->line, this->col}
        });
      default:
        token = TOKEN(char, Curr(), kind, Curr());
        break;
    }
  }
  Next(token.Length());
  return Ok<Token, LexError>(token);
}

template<typename Lambda, typename>
void Lexer::CheckNextAndThen(char check, Lambda then) {
  *(this->file + 1) == check ? this->file++, then(), 0 : 0;
}



