# ifndef NEXTGEN_LEXER_H
# define NEXTGEN_LEXER_H

# include "token.h"
# include "nextgen/support/io.h"

namespace nextgen { namespace jet { using namespace nextgen::core;

  enum ErrorType {
    MalformedUTF8,
    InvalidChar,
    UnexpectedEOF,
    Unreachable,
    IntegerOverflow,
    FloatingPointOverflow
  };

  struct LexError {
    ErrorType type;
    Token::SourceLocation loc;
  };

  class Lexer {
  public:
    using Allocator = nextgen::mem::ArenaSegment;
    using File      = char;

    static constexpr TokenKind Class[256] {

      // Error code
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
      TokenKind::Error, TokenKind::Error,
      TokenKind::String,  // '"'
      TokenKind::Error, TokenKind::Error,
      TokenKind::Percent, // '%'
      TokenKind::AND, // '&'
      TokenKind::Char, // '\''
      TokenKind::Error, TokenKind::Error,
      TokenKind::Star, // '*'
      TokenKind::Plus, // '+'
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


      TokenKind::LBracket, // '['

      TokenKind::Error,

      TokenKind::RBracket, // ']'

      TokenKind::XOR, // '^'

      TokenKind::Identifier,     // '_'

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


    Lexer(Allocator *allocator, File *buf) :
    allocator(allocator), file(buf) {}

    // Peek `n` number of characters in the file buffer.
    NG_INLINE void Peek(size_t n);

    // Get current character that the lexer is at.
    NG_AINLINE char Curr() const;


    // Lex the given File and output a token stream
    Result<nextgen::mem::list<Token>, LexError> Lex();

  private:
    Allocator *allocator;
    File      *file;

    char *pos{file};
    char c{};
  };


}} // namespace nextgen::jet



# endif //NEXTGEN_LEXER_H
