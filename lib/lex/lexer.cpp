# include <nextgen/jet/lex/lexer.h>

using namespace nextgen::jet;


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

// Peek `n` number of characters in the file buffer.
char Lexer::Peek(size_t n) {
  return *(this->file + n);
}

// Get current character that the lexer is at.
char Lexer::Curr() const {
  return *this->file;
}

auto Lexer::Lex() -> Result<nextgen::mem::list<Token>, LexError> {
  auto tokens   = mem::list<Token> { 1000 };
  auto current  = Curr();
  auto kind     = TokenKindClass[current];
  auto token    = Token {};

  switch (kind) {
    case Integer:
      break;
    case String:
      break;
    case Decimal:
      break;
    case Identifier:
      break;
    case Colon:
      break;
    case LessThan:
      break;
    case GreaterThan:
      break;
    case Equals:
      token = GetIfNextIsAssignment(TokenKind::EqualsEquals, this->file, 1);
      break;
    case QuestionMark:
      break;
    case Plus:
      break;
    case Minus:
      break;
    case Slash:
      break;
    case Star:
      break;
    case Char:
      break;
    case Dot:
      break;
    case EqualsEquals:
    case LeftShift:
    case RightShift:
    case Pow:
    case Boolean:
      UNREACHABLE;
      break;
    case Error:
      // TODO: Add Invalid Char Error Here
      return Err<mem::list<Token>, LexError>( LexError {
        ErrorType::InvalidChar,
      });
    default:
      // TODO: This might be really slow for single tokens
      token = Token {
        current,
        Token::BuildTokenKind<char>(this->allocator,kind),
          Token::SourceLocation { this->line, this->col },
          (TokenClassification)(0)
      };
      break;
  }
  tokens.add(token);
  return Ok<mem::list<Token>, LexError>(tokens);
}

template<typename Lambda, typename>
void Lexer::CheckNextAndThen(char check, Lambda then) {
  *(this->file + 1) == check ? this->file++, then(), 0 : 0;
}

template<typename T>
void Lexer::AddToken(TokenKind kind, const char *src, TokenClassification flags,
                     mem::list<Token> &list) {
  auto token = Token {
    src,
    Token::BuildTokenKind<T>(this->allocator, kind),
    Token::SourceLocation { this->line, this->col },
    flags
  };
  list.add(token);
}

Token Lexer::GetIfNextIsAssignment(TokenKind kind, const char *src, size_t
len) {
  size_t length;
  bool assignment = Peek(1) == '=';
  assignment ? this->file++, length = len + 1 : length = len;
  return Token {
    str { src, length },
    Token::BuildTokenKind<const char *>(this->allocator, kind),
    Token::SourceLocation { this->line, this->col },
    (TokenClassification) 0,
    assignment
  };
}


