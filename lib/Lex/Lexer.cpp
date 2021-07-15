# include <nextgen/jet/Lex/Lexer.h>

using namespace nextgen;
using namespace nextgen::jet;

# define TOKEN(type, text, k, value) Token(text, {line, column} \
                                                    ,value, k)

static constexpr TokenKind TokenKindClass[256] {

  // Error code
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error,
  TokenKind::Whitespace, // \a
  TokenKind::Whitespace, // \b
  TokenKind::Whitespace, // \t
  TokenKind::NewLine,    // \n
  TokenKind::Whitespace, // \v
  TokenKind::Error,

  TokenKind::NewLine, // \r

  TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error,


  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,
  TokenKind::Error, TokenKind::Error, TokenKind::Error,

  TokenKind::Whitespace,    // <space>

  TokenKind::ExclamationPoint, // !
  TokenKind::String,        // '"'
  TokenKind::Error, TokenKind::Error,
  TokenKind::Percent,       // '%'
  TokenKind::AND,           // '&'
  TokenKind::Char,          // '\''
  TokenKind::LParenthesis,  // (
  TokenKind::RParenthesis,  // )
  TokenKind::Star,          // '*'
  TokenKind::Plus,          // '+'
  TokenKind::Comma,         // ','

  TokenKind::Minus, // '-'


  TokenKind::Dot, // '.'

  TokenKind::Slash, // '/'


  // Digits
  TokenKind::Integer, TokenKind::Integer, TokenKind::Integer,
  TokenKind::Integer, TokenKind::Integer,
  TokenKind::Integer, TokenKind::Integer, TokenKind::Integer,
  TokenKind::Integer, TokenKind::Integer,


  TokenKind::Colon, // ':'


  TokenKind::SemiColon, // ';'
  TokenKind::LessThan, // '<'
  TokenKind::Equals, // '='
  TokenKind::GreaterThan, // '>'
  TokenKind::QuestionMark, // '?'
  TokenKind::At, // @

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
  { "match"_intern, KeywordMatch },
  { "in"_intern, KeywordIn }
};

/// Determine the validity of parsed string being a Keyword. The assumption
/// made in this function is that `value` is an interned string therefore only
/// direct pointer comparison is made resulting in O(1) comparison.
static TokenKind ReturnValidKeyword(str &value) {
  for (auto i = 0; i < SizeOfArray(Keywords); ++i) {
    if (value == Keywords[i].key)
      return Keywords[i].id;
  }
  return TokenKind::Identifier;
}


auto Lexer::next_token() -> Token {
  TokenKind Kind = TokenKindClass[Curr()];
  Token Instance = Token {};


  goto start;
  start:
  {
    switch (Kind) {

      ///
      /// Integers
      ///
      /// Pattern: Key Lexing Statutes
      ///   • '_' is valid anywhere in the integer as long as it's not the
      ///     first character. Examples:
      ///       • 123_232_2423
      ///       • 234__343
      ///       • 3__23_3_23_2_2
      ///   • Identifiers are not interned for integers. This is because they
      ///     do not provide any reasonable performance gain.
      ///   • All number values with suffixes of: 'e' or 'p' are floating
      ///     point values unless casted (handled in Parser/AST).
      ///
      /// Valid Patterns:
      ///   • 0x123 (Hexadecimal)
      ///   • 12345 (Base 10)
      ///   • 0b011 (Base 2)
      ///   • 0#abc (Base 36)
      ///
      ///
      case Integer: {

        // Integer Base
        int Radix = 10;
        
        // Classification for Unsigned values
        auto Flags = static_cast<TokenClassification>(TokenClassification::Literal);

        // Start of token
        auto Begin = buffer;
        auto BeginCol = column;
        switch (Curr()) {
          case '0':
            switch (Next(1)) {
              case 'b':
                Radix = 2;
                Next(1);
                break;
              case 'x':
                Radix = 16;
                Next(1);
                break;
              case '#':
                Radix = 36;
                Next(1);
                break;
              case '0': case '1': case '2': case '3': case '4': case '5':
              case '6': case '7': case '8': case '9':
                Radix = 8;
                break;
              default: // TODO: Figure what needs to be done here
                break;
            }
            break;
          default:
            break;
        }


        decltype(UINTPTR_MAX) IntegerValue = 0;
        do {

          auto Current = Curr();
          int Digit = IntegerBits[Current];

          if (Digit < 0) {
            break;
          }

          if (Digit >= Radix) {
            diagnostics.build(LexError{
              LexErrorType::DigitOutOfRange,
              {line, column},
              Token(str(Range<const char *>(Begin, buffer)),
                    {line, BeginCol}, TokenKind::Integer),
              {Radix, Digit + 1}
            });
            fatal = true;
            return Instance;
          }

          if (IntegerValue > (UINTPTR_MAX - Digit) / Radix) {
            diagnostics.build(LexError{
              LexErrorType::IntegerOverflow,
              {line, column},
              Token(str(Range<const char *>(Begin, buffer)),
                    {line, BeginCol},
                    TokenKind::Integer),
            });
            fatal = true;
            return Instance;
          }
          IntegerValue = IntegerValue * Radix + Digit;
        } while (Next(1));

        // Token Suffix
        switch (Curr()) { // TODO: Add Implementation
          case 'p': // Power
            break;
          case 'u': // Unsigned
            Flags = TokenClassification::Unsigned;
            Next(1);
            break;
          case 'e': // x10^power
            if (Radix == 16) UNREACHABLE;
            break;
        }

        // End of Token
        auto End = buffer;


        // Important Note: When integer values are lexed, they do not hold
        // a direct value stored in a buffer, rather just a pseudo range value
        // with no allocation is substituted because integer value strings do
        // need to be interned.
        auto ID = str(Range<const char *>(Begin, End));

        Instance = TOKEN(decltype(UINTPTR_MAX), ID, TokenKind::Integer,
                         IntegerValue);
        Instance.setFlag(Flags);
        return Instance;
      }
      case String: {
        auto Begin = this->buffer;
        auto BeginCol = column;

        // FNV-1a Hash on String as Lexing as to not do it twice
        // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_function
        auto val = FNV_OFF;
        char Current = Next(1);

        // Equivalent to C99 String Standard
        auto StringAllocator = allocator->allocator_for_space(4095);

        char *ID = (char*) StringAllocator->current_point();
        size_t Len = 0;

        do {
          Current = Curr();

          if (Current == '\0') {
            diagnostics.build(LexError{
              LexErrorType::MissingClosingDelim,
              {line, column},
              Token(str(Range<const char *>(Begin, buffer)),
                    {line, BeginCol},
                    TokenKind::String),
              {'"'}
            });
            fatal = true;
            return Instance;
          }

          if (Current == '\\') {
            switch (Next(1)) {
              case 'a':
                Current = '\a';
                break;
              case 'b':
                Current = '\b';
                break;
              case 'r':
                Current = '\r';
                break;
              case 'n':
                Current = '\n';
                break;
              case 'v':
                Current = '\v';
                break;
              case 't':
                Current = '\t';
                break;
              case 'x':  {
                auto V1 = IntegerBits[Next(1)];
                if (V1 >= 16) {
                  diagnostics.build(LexError{
                    LexErrorType::HexEscapeOutOfRange,
                    {line, column},
                    Token(str(Range<const char *>(Begin, buffer)),
                          {line, BeginCol},
                          TokenKind::String),
                    {V1 + 1} // Pass in the invalid digit for the escape
                  });
                  fatal = true;
                  return Instance;
                }
                auto V2 = IntegerBits[Next(1)];
                if (V2 >= 16) {
                  diagnostics.build(LexError{
                    LexErrorType::HexEscapeOutOfRange,
                    {line, column},
                    Token(str(Range<const char *>(Begin, buffer)),
                          {line, BeginCol},
                          TokenKind::String),
                    {V2 + 1} // Pass in the invalid digit for the escape
                  });
                  fatal = true;
                  return Instance;
                }
                
                // Unroll Loop
                int Value = V1;
                Value = Value * 16 + V2;

                // @Safety Guaranteed! We know for a fact that this conversion
                // is safe because of the validity.
                Current = (char) Value;
                break;
              }
              case 'u': // TODO: IDK how to do this properly but ... Unicode Char
                break;
              default:
                diagnostics.build(LexError{
                  LexErrorType::InvalidStringEscape,
                  {line, column},
                  Token(str(Range<const char *>(Begin, buffer)),
                        {line, BeginCol},
                        TokenKind::String),
                });
                fatal = true;
                return Instance;
            }
          }

          val ^= Current;
          val *= FNV_PRIME;

          ID[Len++] = Current;


        } while (Next(1) != '"');

        ID[Len++] = '\0';

        auto S = str(ID, Len);
        S.setHash(val);
        auto Intern = StringCache::InsertOrGet(S);

        Instance = TOKEN(const char *, Intern, TokenKind::String, "");
        Instance.setFlag(TokenClassification::Literal);
        Next(1); // Skip the last '"'
        return Instance;
      }
      case Decimal:
        break;
      case Identifier: {


        auto Begin = this->buffer;


        // FNV-1a Hash on String as Lexing as to not do it twice
        // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_function
        auto val = FNV_OFF;
        char Current;
        do {
          Current = Curr();

          val ^= Current;
          val *= FNV_PRIME;


        } while (TokenKindClass[Next(1)] == TokenKind::Identifier);

        auto End = this->buffer;

        auto S = str(Begin, End-Begin);
        S.setHash(val);
        auto Intern = StringCache::InsertOrGet(S);

        // Ensure appropriate kind is assigned
        auto Type = ReturnValidKeyword(Intern);

        Instance = TOKEN(const char *, Intern, Type, "");
        if (Type != TokenKind::Identifier)
          Instance.setFlag(TokenClassification::Keyword);
        Instance.setFlag(TokenClassification::Literal);

        return Instance;
      }
      case LessThan: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::LessThan, TokenKind::Equals):
            Instance = TOKEN(const char *, "<=", TokenKind::LessThanEquals,
                             "<=");
            Instance.assignment = true;
            break;
          group(TokenKind::LessThan, TokenKind::LessThan): // << <--
            Kind = TokenKind::LeftShift;
            goto start;
          default:
            Instance = TOKEN(char, "<", TokenKind::LessThan, "<");
            break;
        }

        auto s2 = Peek(1);
        match ('<', s2) {
          group('<', '='):
            Instance = TOKEN(const char *, "<=", TokenKind::LessThanEquals,
                             "<=");
            Instance.assignment = true;
            break;
          group('<', '<'): // << <--
            Kind = TokenKind::LeftShift;
            goto start;
          default:
            Instance = TOKEN(char, "<", TokenKind::LessThan, "<");
            break;
        }
        if (s2 == '=') {
          Instance = TOKEN(const char *, "<=", TokenKind::LessThanEquals,
                           "<=");
          Instance.assignment = true;
        }
        break;
      }
      case LeftShift: {
        auto s1 = TokenKindClass[Peek(2)];
        match (Kind, s1) {
          group(TokenKind::LeftShift, TokenKind::Equals):
            Instance.assignment = true;
            Instance = TOKEN(const char *, "<<=", TokenKind::LeftShiftEquals,
                             "<<=");
            break;
          default:
            Instance = TOKEN(const char *, "<<", TokenKind::LeftShift, "<<");
            break;
        }
        break;
      }
      case RightShift: {
        auto s1 = TokenKindClass[Peek(2)];
        match (Kind, s1) {
          group(TokenKind::RightShift, TokenKind::Equals):
            Instance.assignment = true;
            Instance = TOKEN(const char *, ">>=", TokenKind::RightShiftEquals,
                             ">>=");
            break;
          default:
            Instance = TOKEN(const char *, ">>", TokenKind::RightShift, ">>");
            break;
        }
        break;
      }
      case GreaterThan: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::GreaterThan, TokenKind::Equals):
            Instance = TOKEN(const char *, ">=", TokenKind::GreaterThanEquals,
                             ">=");
            Instance.assignment = true;
            break;
          group(TokenKind::GreaterThan, TokenKind::GreaterThan):
            Kind = TokenKind::RightShift;
            goto start;
          default:
            Instance = TOKEN(char, ">", TokenKind::GreaterThan, ">");
            break;
        }
        break;
      }
      case Equals: {
        auto s1 = TokenKindClass[Peek(1)];
        if (s1 == TokenKind::Equals) {
          Instance = TOKEN(const char *, "==", TokenKind::EqualsEquals, "==");
        } else {
          Instance = TOKEN(char, "=", TokenKind::Equals, "=");
        }
        break;
      }
      case QuestionMark: {
        auto s1 = TokenKindClass[Peek(1)];
        if (s1 == TokenKind::QuestionMark) {
          Instance = TOKEN(const char *, "??", TokenKind::Then, "??");
        } else {
          Instance = TOKEN(char, "?", TokenKind::QuestionMark, "?");
        }
        break;
      }
      case Plus: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Plus, TokenKind::Plus):
            Instance = TOKEN(const char *, "++", TokenKind::PlusPlus, "++");
            break;
          group(TokenKind::Plus, TokenKind::Equals):
            Instance = TOKEN(const char *, "+=", TokenKind::PlusEquals, "+=");
            Instance.assignment = true;
            break;
          default:
            Instance = TOKEN(char, "+", TokenKind::Plus, "+");
            break;
        }
        break;
      }
      case Minus: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Minus, TokenKind::Minus):
            Instance = TOKEN(const char *, "--", TokenKind::MinusMinus, "--");
            break;
          group(TokenKind::Minus, TokenKind::Equals):
            Instance = TOKEN(const char *, "-=", TokenKind::MinusEquals, "-=");
            Instance.assignment = true;
            break;
          default:
            Instance = TOKEN(char, "-", TokenKind::Minus, "-");
            break;
        }
        break;
      }
      case Slash: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Slash, TokenKind::Equals):
            Instance = TOKEN(const char *, "/=", TokenKind::DivEquals, "/=");
            Instance.assignment = true;
            break;
          default:
            Instance = TOKEN(char, "/", TokenKind::Star, "/");
            break;
        }
        break;
      }
      case Star: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Star, TokenKind::Star):
            Kind = TokenKind::Pow;
            goto start;
          group(TokenKind::Star, TokenKind::Equals):
            Instance = TOKEN(const char *, "*=", TokenKind::MulEquals, "*=");
            Instance.assignment = true;
            break;
          default:
            Instance = TOKEN(const char *, "*", TokenKind::Star, "*");
            break;
        }
        break;
      }
      case Pow: {
        auto s1 = TokenKindClass[Peek(2)];
        match (Kind, s1) {
          group(TokenKind::Pow, TokenKind::Equals):
            Instance.assignment = true;
            Instance = TOKEN(const char *, "**=", TokenKind::PowEquals, "**=");
            break;
          default:
            Instance = TOKEN(const char *, "**", TokenKind::Pow, "**");
            break;
        }
        break;
      }
      case AND: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::AND, TokenKind::Equals):
            Instance = TOKEN(const char *, "&=", TokenKind::ANDEquals, "&=");
            Instance.assignment = true;
            break;
          default:
            Instance = TOKEN(char, "&", TokenKind::AND, "&");
            break;
        }
        break;
      }
      case Pipe: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Pipe, TokenKind::Equals):
            Instance = TOKEN(const char *, "|=", TokenKind::OREquals, "|=");
            Instance.assignment = true;
            break;
          default:
            Instance = TOKEN(char, "|", TokenKind::Pipe, "|");
            break;
        }
        break;
      }
      case XOR: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::XOR, TokenKind::Equals):
            Instance = TOKEN(const char *, "^=", TokenKind::XOREquals, "^=");
            Instance.assignment = true;
            break;
          default:
            Instance = TOKEN(char, "^", TokenKind::XOR, "^");
            break;
        }
        break;
      }
      case NewLine: // \n or \r
        SkipNewLine();
        Kind = TokenKindClass[Curr()];
        goto start; // Does this so 2 function calls aren't wasted.
      case Char: {
        auto Begin = buffer;
        auto BeginCol = column;
        Instance = TOKEN(char, Next(1), TokenKind::Char, Curr());
        if (TokenKindClass[Next(1)] != TokenKind::Char) {
          diagnostics.build(LexError{
            LexErrorType::MissingClosingDelim,
            {line, column}, // 'x[?] <-- Missing Closing!
            Token(str(Range<const char *>(Begin, buffer)),
                  {line, BeginCol},
                  TokenKind::String),
            {'\''}
          });
          fatal = true;
        }
        Instance.setFlag(TokenClassification::Literal);
        break;
      }
      case Whitespace: // Skip whitespace
        Next(1);
        Kind = TokenKindClass[Curr()];
        goto start;
      case Dot: {
        auto Begin = buffer;
        auto BeginCol = column;

        auto s1 = TokenKindClass[Peek(1)];
        auto s2 = TokenKindClass[Peek(2)];
        auto s3 = TokenKindClass[Peek(3)];

        match (Kind, s1) {
          group(TokenKind::Dot, TokenKind::Dot):
            Instance = s2 == TokenKind::Dot ?
                       (TOKEN(const char *, "...",
                           TokenKind::Ellipsis, "...")) :
                       (TOKEN(const char *, "..",
                           TokenKind::RangeSpan, ".."));
            break;
          default:
            Instance = TOKEN(char, ".", TokenKind::Dot, ".");
            break;
        }
        // Cannot match token "...[.]" <-- Invalid Dot Here!
        if (s3 == TokenKind::Dot && Instance.getKind() == TokenKind::Ellipsis) {
          diagnostics.build(LexError{
            LexErrorType::InvalidDot,
            {line, column + 1},
            Token(str(Range<const char *>(Begin, buffer)),
                  {line, BeginCol},
                  TokenKind::Ellipsis)
          });
          fatal = true;
        }

        break;
      }
        // Cannot be generated by the TokenKind table, assigned after checks
      case EqualsEquals:
      case Boolean:
        UNREACHABLE;
        break;
      case LParenthesis:
        Instance = TOKEN(const char *, "(", Kind, ")");
        break;
      case RParenthesis:
        Instance = TOKEN(const char *, ")", Kind, ")");
        break;
      case LBracket:
        Instance = TOKEN(const char *, "[", Kind, "[");
        break;
      case RBracket:
        Instance = TOKEN(const char *, "]", Kind, "]");
        break;
      case LCurlyBrace:
        Instance = TOKEN(const char *, "{", Kind, "{");
        break;
      case RCurlyBrace:
        Instance = TOKEN(const char *, "}", Kind, "}");
        break;
      case NOT:
        Instance = TOKEN(const char *, "~", Kind, "~");
        break;
      case Colon:
        Instance = TOKEN(const char *, ":", Kind, ":");
        break;
      case SemiColon:
        Instance = TOKEN(const char *, ";", Kind, ";");
        break;
      case ExclamationPoint:
        Instance = TOKEN(const char *, "!", Kind, "!");
        break;
      case At:
        Instance = TOKEN(const char *, "@", Kind, "@");
        break;
      case Percent:
        Instance = TOKEN(const char *, "%", Kind, "%");
        break;
      case Comma:
        Instance = TOKEN(const char *, ",", Kind, ",");
        break;
      case Error:
        if (Curr() == '\0') {
          buf_pos++;
          return Token("\0", {line, column},
                                           TokenKind::EOFToken);
        }
        diagnostics.build(LexError{
          LexErrorType::InvalidChar,
          {line, column}
        });
        fatal = true;
      default:
        //Instance = TOKEN(char, Curr(), kind, Curr());
        break;
    }
  }
  Next(Instance.len());
  return Instance;
}

void Lexer::PrintNextToken() {
  TokenKind Kind = TokenKindClass[Curr()];
  Token Instance = Token {};

  goto start;
  start:
  {
    if (!buffer)
      return;
    switch (Kind) {

      case Integer: {

        // Start of token
        auto Begin = buffer;

        switch (Curr()) {
          case '0':
            switch (Next(1)) {
              case 'b':
                Next(1);
                break;
              case 'x':
                Next(1);
                break;
              case '#':
                Next(1);
                break;
              case '0': case '1': case '2': case '3': case '4': case '5':
              case '6': case '7': case '8': case '9':
                break;
              default:
                break;
            }
            break;
          default:
            break;
        }

        do {

          auto Current = Curr();
          int Digit = IntegerBits[Current];

          if (Digit < 0) {
            break;
          }

        } while (Next(1));

        // Token Suffix
        switch (Curr()) { // TODO: Add Implementation
          case 'p': // Power
            break;
          case 'u': // Unsigned
            break;
          case 'e': // x10^power
            break;
        }

        auto End = buffer;

        auto Print = str(Begin, End - Begin);
        Console::Log(Colors::BLUE, Print, Colors::RESET);

        break;
      }
      case String: {
        auto Begin = this->buffer;
        int Offset = 1;
        Console::Log(Colors::GREEN);

        char Current;
        do {
          Current = Curr();

          if (Current == '\0') {
            Offset = 0;
            break;
          }
        } while (Next(1) != '"');


        auto End = this->buffer;

        auto Print = str(Begin, End - Begin + Offset);
        Console::Log(Print, Colors::RESET);

        Next(1); // Skip the last '"'
        break;
      }
      case Decimal:
        break;
      case Identifier: { // Maybe : Identifier length no more 256 characters?


        auto Begin = this->buffer;

        // FNV-1a Hash on String as Lexing as to not do it twice
        // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_function
        auto val = FNV_OFF;
        do {

          val ^= Curr();
          val *= FNV_PRIME;
        } while (TokenKindClass[Next(1)] == TokenKind::Identifier);


        auto End = this->buffer;

        auto S = str(Begin, End-Begin);
        S.setHash(val);
        auto Intern = StringCache::InsertOrGet(S);

        // Ensure appropriate kind is assigned
        auto Type = ReturnValidKeyword(Intern);

        if (Type != TokenKind::Identifier) {
          Console::Log(Colors::RED, Intern, Colors::RESET);
        } else {

          Console::Log(Colors::YELLOW, Intern, Colors::RESET);
        }
        break;
      }
      case LessThan: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::LessThan, TokenKind::Equals):
            Console::Log("<=");
            Next(2);
            break;
          group(TokenKind::LessThan, TokenKind::LessThan):
            Kind = TokenKind::LeftShift;
            goto start;
          default:
            Console::Log("<");
            Next(1);
            break;
        }
        break;
      }
      case LeftShift: {
        auto s1 = TokenKindClass[Peek(2)];
        match (Kind, s1) {
          group(TokenKind::LeftShift, TokenKind::Equals):
            Console::Log("<<=");
            Next(3);
            break;
          default:
            Console::Log("<<");
            Next(2);
            break;
        }
        break;
      }
      case RightShift: {
        auto s1 = TokenKindClass[Peek(2)];
        match (Kind, s1) {
          group(TokenKind::RightShift, TokenKind::Equals):
            Console::Log(">>=");
            Next(3);
            break;
          default:
            Console::Log(">>");
            Next(2);
            break;
        }
        break;
      }
      case GreaterThan: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::GreaterThan, TokenKind::Equals):
            Console::Log(">=");
            Next(2);
            break;
          group(TokenKind::GreaterThan, TokenKind::GreaterThan):
            Kind = TokenKind::RightShift;
            goto start;
          default:
            Console::Log(">");
            Next(1);
            break;
        }
        break;
      }
      case Equals: {
        auto s1 = TokenKindClass[Peek(1)];
        if (s1 == TokenKind::Equals) {
          Console::Log("==");
          Next(1);
        } else {
          Console::Log("=");
        }
        Next(1);
        break;
      }
      case QuestionMark: {
        auto s1 = TokenKindClass[Peek(1)];
        if (s1 == TokenKind::QuestionMark) {
          Console::Log("??");
          Next(2);
        } else {
          Console::Log("?");
          Next(1);
        }
        break;
      }
      case Plus: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Plus, TokenKind::Plus):
            Console::Log("++");
            Next(2);
            break;
          group(TokenKind::Plus, TokenKind::Equals):
            Console::Log("+=");
            Next(2);
            break;
          default:
            Console::Log("+");
            Next(1);
            break;
        }
        break;
      }
      case Minus: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Minus, TokenKind::Minus):
            Console::Log("--");
            Next(2);
            break;
          group(TokenKind::Minus, TokenKind::Equals):
            Console::Log("-=");
            Next(2);
            break;
          default:
            Console::Log("-");
            Next(1);
            break;
        }
        break;
      }
      case Slash: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Slash, TokenKind::Equals):
            Instance = TOKEN(const char *, "/=", TokenKind::DivEquals, "/=");
            Instance.assignment = true;
            break;
          default:
            Instance = TOKEN(const char *, "/", TokenKind::Slash, "/");
            break;
        }
        break;
      }
      case Star: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Star, TokenKind::Star):
            Kind = TokenKind::Pow;
            goto start;
          group(TokenKind::Star, TokenKind::Equals):
            Console::Log("*=");
            break;
          default:
            Console::Log('*');
            break;
        }
        break;
      }
      case Pow: {
        auto s1 = TokenKindClass[Peek(2)];
        match (Kind, s1) {
          group(TokenKind::Pow, TokenKind::Equals):
            Console::Log("**=");
            Next(3);
            break;
          default:
            Console::Log("**");
            Next(2);
            break;
        }
        break;
      }
      case AND: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::AND, TokenKind::Equals):
            Console::Log("/=");
            Next(2);
            break;
          default:
            Console::Log('/');
            Next(1);
            break;
        }
        break;
      }
      case Pipe: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Pipe, TokenKind::Equals):
            Console::Log("|=");
            Next(2);
            break;
          default:
            Console::Log('|');
            Next(1);
            break;
        }
        break;
      }
      case XOR: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::XOR, TokenKind::Equals):
            Console::Log("^=");
            Next(2);
            break;
          default:
            Console::Log('^');
            Next(1);
            break;
        }
        break;
      }
      case NewLine: // \n or \r
        Console::Log(Curr());
        SkipNewLine();
        Kind = TokenKindClass[Curr()];
        goto start; // Does this so 2 function calls aren't wasted.
      case Char:
        Console::Log(Colors::GREEN, "'", Next(1), Colors::RESET);
        if (Peek(1) == '\'')
          Console::Log("'");
        Next(1);
        break;
      case Whitespace: // Skip whitespace
        Console::Log(Curr());
        Next(1);
        Kind = TokenKindClass[Curr()];
        goto start;
      case Dot: {
        auto s1 = TokenKindClass[Peek(1)];
        auto s2 = TokenKindClass[Peek(2)];

        match (Kind, s1) {
          group(TokenKind::Dot, TokenKind::Dot):
            s2 == TokenKind::Dot ?
                Next(3), Console::Log("...") :
                (Next(2), Console::Log(".."));
            break;
          default:
            Console::Log(".");
            Next(1);
            break;
        }
        break;
      }
      // Cannot be generated by the TokenKind table, assigned after checks
      case EqualsEquals:
      case Boolean:
        UNREACHABLE;
        break;
      case LParenthesis:
      case RParenthesis:
      case LBracket:
      case RBracket:
      case LCurlyBrace:
      case RCurlyBrace:
      case NOT:
      case Colon:
      case SemiColon:
      case ExclamationPoint:
      case At:
      case Comma:
        Console::Log(Curr());
        Next(1);
        break;
      case Error:
        buffer = nullptr;
        break;
      default:
        break;
    }
  }
}
