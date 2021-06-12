# include <nextgen/jet/Lex/Lexer.h>

using namespace nextgen;
using namespace nextgen::jet;

# define TOKEN(type, text, k, value) Token::New(text, {CurrentLine, CurrentColumn} \
                                                    ,value, k)




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

/// Determine the validity of parsed string being a Keyword. The assumption
/// made in this function is that `value` is an interned string therefore only
/// direct pointer comparison is made resulting in O(1) comparison.
static auto ReturnValidKeyword(str &value, TokenKind else_) -> TokenKind {
  for (auto i = 0; i < SizeOfArray(Keywords); ++i) {
    if (value == Keywords[i].key)
      return Keywords[i].id;
  }
  return else_;
}




auto Lexer::NextToken() -> Result<Token, LexError> {
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

        // FNV-1a Hash Value
        auto Hash  = FNV_OFF;

        // Classification for Unsigned Values
        TokenClassification Flags = static_cast<TokenClassification>(0);

        // Start of token
        auto Begin = Buffer;
        auto BeginCol = CurrentColumn;
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

          // Calculate Identifier FNV-1a Hash
          Hash ^= Current;
          Hash *= FNV_PRIME;

          int digit = IntegerBits[Current];

          if (digit < 0) {
            break;
          }

          if (digit >= Radix) {
            return Err<Token, LexError>( LexError {
              LexErrorType::DigitOutOfRange,
              { CurrentLine, CurrentColumn },
              Token::New(str(Range<const char *>(Begin, Buffer)),
                         {CurrentLine, BeginCol}, TokenKind::Integer)
            });
          }
          if (IntegerValue > (UINTPTR_MAX - digit) / Radix) {
            return Err<Token, LexError>( LexError {
              LexErrorType::IntegerOverflow,
              { CurrentLine, CurrentColumn },
              Token::New(str(Range<const char *>(Begin, Buffer)),
                         {CurrentLine, BeginCol}, TokenKind::Integer)
            });
          }
          IntegerValue = IntegerValue * Radix + digit;
        } while (Next(1));

        // Token Suffix
        switch (Curr()) { // TODO: Add Implementation
          case 'p': // Power
            break;
          case 'u': // Unsigned
            Flags = TokenClassification::Unsigned;
            break;
          case 'e': // x10^power
            break;
        }

        // End of Token
        auto End = Buffer;


        // Important Note: When integer values are lexed, they do not hold
        // a direct value stored in a buffer, rather just a pseudo range value
        // with no allocation is substituted because integer value strings do
        // need to be interned.
        auto ID = str(Range<const char *>(Begin, End));

        Instance = TOKEN(decltype(UINTPTR_MAX), ID, TokenKind::Integer,
                         IntegerValue);
        Instance.setFlag(Flags);
        return Ok<Token, LexError>(Instance);
      }
      case String:
        break;
      case Decimal:
        break;
      case Identifier: { // Identifier are no more than 256 characters


        auto Begin = this->Buffer;


        // FNV-1a Hash on String as Lexing as to not do it twice
        // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_function
        auto val = FNV_OFF;
        char Current;
        do {
          Current = Curr();

          val ^= Current;
          val *= FNV_PRIME;


        } while (TokenKindClass[Next(1)] == TokenKind::Identifier);


        auto End = this->Buffer;

        auto S = (str) Range<const char *>(Begin, End);
        S.setHash(val);
        auto Intern = StringInterner::InsertOrGet(Memory, S);

        // Ensure appropriate kind is assigned
        auto Type = ReturnValidKeyword(Intern,TokenKind::Identifier);

        Instance = TOKEN(const char *, Intern, Type, "");
        if (Type != TokenKind::Identifier)
          Instance.setFlag(TokenClassification::Keyword);

        return Ok<Token, LexError>(Instance);
      }
      case LessThan: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::LessThan, TokenKind::Equals):
            Instance = TOKEN(const char *, "<=", TokenKind::LessThan, "<=");
            Instance.assignment = true;
            break;
          group(TokenKind::LessThan, TokenKind::LessThan):
            Kind = TokenKind::LeftShift;
            goto start;
          default:
            Instance = TOKEN(char, '<', TokenKind::LessThan, '<');
            break;
        }
        break;
      }
      case LeftShift: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::LeftShift, TokenKind::Equals):
            Instance.assignment = true;
            Instance = TOKEN(const char *, "<<=", TokenKind::LeftShift, "<<=");
          default:
            Instance = TOKEN(const char *, "<<", TokenKind::LeftShift, "<<");
            break;
        }
        break;
      }
      case RightShift: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::RightShift, TokenKind::Equals):
            Instance.assignment = true;
            Instance = TOKEN(const char *, ">>=", TokenKind::RightShift, ">>=");
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
            Instance = TOKEN(const char *, ">=", TokenKind::GreaterThan, ">=");
            Instance.assignment = true;
            break;
          group(TokenKind::GreaterThan, TokenKind::GreaterThan):
            Kind = TokenKind::LeftShift;
            goto start;
          default:
            Instance = TOKEN(char, '>', TokenKind::GreaterThan, '>');
            break;
        }
        break;
      }
      case Equals: {
        auto s1 = TokenKindClass[Peek(1)];
        if (s1 == TokenKind::Equals) {
          Instance = TOKEN(const char *, "==", TokenKind::EqualsEquals, "==");
        } else {
          Instance = TOKEN(char, '=', TokenKind::Equals, '=');
        }
        break;
      }
      case QuestionMark: {
        auto s1 = TokenKindClass[Peek(1)];
        if (s1 == TokenKind::QuestionMark) {
          Instance = TOKEN(const char *, "??", TokenKind::Then, "??");
        } else {
          Instance = TOKEN(char, '?', TokenKind::QuestionMark, '?');
        }
        break;
      }
      case Plus: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Plus, TokenKind::PlusPlus):
            Instance = TOKEN(const char *, "++", TokenKind::PlusPlus, "++");
            break;
          group(TokenKind::Plus, TokenKind::Equals):
            Instance = TOKEN(const char *, "+=", TokenKind::Plus, "+=");
            Instance.assignment = true;
          default:
            Instance = TOKEN(char, '+', TokenKind::Plus, '+');
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
            Instance = TOKEN(const char *, "-=", TokenKind::MinusMinus, "-=");
            Instance.assignment = true;
          default:
            Instance = TOKEN(char, '-', TokenKind::Minus, '-');
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
        Instance = TOKEN(char, Next(1), TokenKind::Char, Curr());
        if (TokenKindClass[Next(1)] != TokenKind::Char) {
          return Err<Token, LexError>(LexError{
            LexErrorType::MissingClosingForChar,
            {CurrentLine, CurrentColumn} // 'x[?] <-- Missing Closing!
          });
        }
        break;
      case Whitespace: // Skip whitespace
        Next(1);
        Kind = TokenKindClass[Curr()];
        goto start;
      case Dot: {
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
            Instance = TOKEN(char, '.', TokenKind::Dot, '.');
            break;
        }
        // Cannot match token "...[.]" <-- Invalid Dot Here!
        if (s3 == TokenKind::Dot && Instance.getKind() == TokenKind::Ellipsis) {
          return Err<Token, LexError>(LexError{
            LexErrorType::InvalidDot,
            {CurrentLine, CurrentColumn + 1}
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
          LexErrorType::InvalidChar,
          {CurrentLine, CurrentColumn}
        });
      default:
        Instance = TOKEN(char, Curr(), Kind, Curr());
        break;
    }
  }
  Next(Instance.Length());
  return Ok<Token, LexError>(Instance);
}

void Lexer::PrintNextToken() {
  TokenKind Kind = TokenKindClass[Curr()];
  Token Instance = Token {};

  goto start;
  start:
  {
    switch (Kind) {

      case Integer: {

        // Start of token
        auto Begin = Buffer;

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
          int digit = IntegerBits[Current];

          if (digit < 0) {
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

        auto End = Buffer;

        std::string Print = std::string(Begin, End);
        Console::Log(Colors::BLUE, Print, Colors::RESET);

        break;
      }
      case String:
        break;
      case Decimal:
        break;
      case Identifier: { // Maybe : Identifier length no more 256 characters?


        auto Begin = this->Buffer;

        // FNV-1a Hash on String as Lexing as to not do it twice
        // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_function
        auto val = FNV_OFF;
        do {

          val ^= Curr();
          val *= FNV_PRIME;
        } while (TokenKindClass[Next(1)] == TokenKind::Identifier);


        auto End = this->Buffer;

        auto Val = std::string(Begin, End); // TODO: Fix Me ASAP
        auto S = str(Val);
        S.setHash(val);
        auto Intern = StringInterner::Intern(S);

        // Ensure appropriate kind is assigned
        auto Type = ReturnValidKeyword(Intern,TokenKind::Identifier);
        if (Type == TokenKind::Identifier) {
          Console::Log(Colors::YELLOW, Intern, Colors::RESET);
        } else {
          Console::Log(Colors::RED, Intern, Colors::RESET);
        }
        break;
      }
      case LessThan: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::LessThan, TokenKind::Equals):
            Console::Log("<=");
            break;
          group(TokenKind::LessThan, TokenKind::LessThan):
            Kind = TokenKind::LeftShift;
            goto start;
          default:
            Console::Log("<");
            break;
        }
        break;
      }
      case LeftShift: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::LeftShift, TokenKind::Equals):
            Console::Log("<<=");
          default:
            Console::Log("<<");
            break;
        }
        break;
      }
      case RightShift: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::RightShift, TokenKind::Equals):
            Console::Log(">>=");
          default:
            Console::Log(">>");
            break;
        }
        break;
      }
      case GreaterThan: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::GreaterThan, TokenKind::Equals):
            Console::Log(">=");
            break;
          group(TokenKind::GreaterThan, TokenKind::GreaterThan):
            Kind = TokenKind::LeftShift;
            goto start;
          default:
            Console::Log(">");
            break;
        }
        break;
      }
      case Equals: {
        auto s1 = TokenKindClass[Peek(1)];
        if (s1 == TokenKind::Equals) {
          Console::Log("==");
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
        } else {
          Console::Log("?");
        }
        break;
      }
      case Plus: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Plus, TokenKind::PlusPlus):
            Console::Log("++");
            break;
          group(TokenKind::Plus, TokenKind::Equals):
            Console::Log("+=");
          default:
            Console::Log("+");
            break;
        }
        break;
      }
      case Minus: {
        auto s1 = TokenKindClass[Peek(1)];
        match (Kind, s1) {
          group(TokenKind::Minus, TokenKind::Minus):
            Console::Log("--");
            break;
          group(TokenKind::Minus, TokenKind::Equals):
            Console::Log("-=");
            break;
          default:
            Console::Log("-");
            break;
        }
        break;
      }
      case Slash:
        break;
      case Star:
        break;
      case NewLine: // \n or \r
        Console::Log(Curr());
        SkipNewLine();
        goto start; // Does this so 2 function calls aren't wasted.
      case Char:
        Console::Log(Colors::GREEN, "'", Next(1), "'", Colors::RESET);
        break;
      case Whitespace: // Skip whitespace
        Console::Log(Curr());
        Next(1);
        Kind = TokenKindClass[Curr()];
        goto start;
      case Dot: {
        auto s1 = TokenKindClass[Peek(1)];
        auto s2 = TokenKindClass[Peek(2)];
        auto s3 = TokenKindClass[Peek(3)];

        match (Kind, s1) {
          group(TokenKind::Dot, TokenKind::Dot):
            s2 == TokenKind::Dot ?
                       Console::Log("...") :
                       Console::Log("..");
            break;
          default:
            Console::Log(".");
            break;
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
        Buffer = 0;
        break;
      default:
        //Console::Log(Curr());
        break;
    }
  }
  Next(Instance.Length());
}




