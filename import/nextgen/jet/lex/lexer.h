# ifndef NEXTGEN_LEXER_H
# define NEXTGEN_LEXER_H

# include "token.h"
namespace nextgen { namespace jet { using namespace nextgen::core;

  enum ErrorType {
    MalformedUTF8,
    InvalidChar,
    UnexpectedEOF,
    Unreachable,
    IntegerOverflow,
    FloatingPointOverflow,

    DigitOutOfRange,
    DoubleUnderscore,

    MissingClosingForChar,
    MissingClosingForString,

    InvalidDot,
    InvalidNumberValue

  };



  struct LexError {
    ErrorType type;
    Token::SourceLocation location;
  };


  class Lexer {
  public:
    using Allocator = mem::ArenaSegment;
    using File      = char;

    Lexer(Allocator *allocator, const File *buf, const size_t len)
    : allocator(allocator), file(buf), size(len) {}

    // Get the next valid Token in the File stream
    auto NextToken() -> Result<Token, LexError>;


    // Peek `n` number of characters in the file buffer.
    NG_INLINE auto Peek(size_t n) -> char {
      return *(this->file + n);
    }

    // Get current character that the lexer is at.
    NG_AINLINE auto Curr() -> char const {
      return *this->file;
    }

    // Move the lexer forward while modifying relevant values
    NG_INLINE auto Next(size_t n) -> char {

      if (this->pos >= this->size) {
        // TODO: Add Immediate Panic Error Here!
        return *(this->file + (n - 1));
      }

      this->col+=n;
      this->pos+=n;
      this->file+=n;
      return *this->file;
    }

  private:

    // Handle new line escape in file
    NG_INLINE void SkipNewLine() {
      this->line++;
      auto current = Curr();
      auto next    = Next(1);
      if (next == '\n' || next == '\r' && next != current)
        ++this->line, Next(1);
    }

    // Execute an action if the next char is equal to `check`
    template<LAMBDA(void, void)>
    NG_INLINE void CheckNextAndThen(char check, Lambda then);


    Allocator  *allocator;  // Token Allocator
    const File *file; // Source Buffer
    const size_t size; // Size of File

    size_t line = 1;
    size_t col  = 1;
    size_t pos  = 0;
  };


}} // namespace nextgen::jet



# endif //NEXTGEN_LEXER_H
