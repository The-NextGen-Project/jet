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

    enum CharacterClass {
      ID = 0,
      Number = 1,
      Single = 2,
      Char2 = 3,
      Char3 = 4,
      Str = 5,
      Real = 6,
      Error = 777,
    };


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
      CharacterClass::Char2, // '%'
      CharacterClass::Char2, // '&'
      CharacterClass::Str, // '\''
      TokenKind::Error, TokenKind::Error,
      CharacterClass::Char3, // '*'
      CharacterClass::Char2, // '+'
      TokenKind::Error,

      CharacterClass::Char2, // '-'


      CharacterClass::Real, // '.'

      CharacterClass::Char2, // '/'


      // Digits
      TokenKind::Integer, TokenKind::Integer, TokenKind::Integer,
      TokenKind::Integer, TokenKind::Integer,
      TokenKind::Integer, TokenKind::Integer, TokenKind::Integer,
      TokenKind::Integer, TokenKind::Integer,


      CharacterClass::Char2, // ':'

      // Error Code
      TokenKind::Error,
      CharacterClass::Char3, // '<'
      CharacterClass::Char2, // '='
      CharacterClass::Char3, // '>'
      CharacterClass::Char2, // '?'
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
    Result<List<Token>, LexError> lex();

  private:
    Allocator *allocator;
    File      *file;

    char *pos{file};
    char c{};
    CharacterClass clz;
  };


}} // namespace nextgen::jet



# endif //NEXTGEN_LEXER_H
