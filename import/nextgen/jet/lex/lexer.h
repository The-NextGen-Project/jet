# ifndef NEXTGEN_LEXER_H
# define NEXTGEN_LEXER_H

# include "token.h"
# include "nextgen/support/io.h"

namespace nextgen { namespace jet {
using namespace nextgen::core;

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


    static constexpr CharacterClass Class[256]{

      // Error code
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error,


      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Str,  // '"'
      CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Char2, // '%'
      CharacterClass::Char2, // '&'
      CharacterClass::Str, // '\''
      CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Char3, // '*'
      CharacterClass::Char2, // '+'
      CharacterClass::Error,

      CharacterClass::Char2, // '-'


      CharacterClass::Real, // '.'

      CharacterClass::Char2, // '/'


      // Digits
      CharacterClass::Number, CharacterClass::Number, CharacterClass::Number,
      CharacterClass::Number, CharacterClass::Number,
      CharacterClass::Number, CharacterClass::Number, CharacterClass::Number,
      CharacterClass::Number, CharacterClass::Number,


      CharacterClass::Char2, // ':'

      // Error Code
      CharacterClass::Error,
      CharacterClass::Char3, // '<'
      CharacterClass::Char2, // '='
      CharacterClass::Char3, // '>'
      CharacterClass::Char2, // '?'
      CharacterClass::Error,

      // Uppercase letters
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID,


      CharacterClass::Single, // '['

      CharacterClass::Error,

      CharacterClass::Single, // ']'

      CharacterClass::Char2, // '^'

      CharacterClass::ID,     // '_'

      CharacterClass::Error,

      // Lowercase letters
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID, CharacterClass::ID,
      CharacterClass::ID, CharacterClass::ID,


      CharacterClass::Single, // '{'
      CharacterClass::Char2, // '|'
      CharacterClass::Single, // '}'
      CharacterClass::Char2, // '~'

      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error, CharacterClass::Error,
      CharacterClass::Error, CharacterClass::Error,
    };


    Lexer(Allocator *allocator, File *buf) : allocator(allocator),
                                             file(buf) {}

    // Peek `n` number of characters in the file buffer.
    NG_INLINE void peek(size_t n);

    // Get current character that the lexer is at.
    NG_AINLINE char curr() const;


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
