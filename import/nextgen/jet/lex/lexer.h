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
    Token::SourceLocation location;
  };

  class Lexer {
  public:
    using Allocator = nextgen::mem::ArenaSegment;
    using File      = char;

    Lexer(Allocator *allocator, const File *buf)
    : allocator(allocator), file (buf) {}

    // Lex the given File and output a token stream
    Result<nextgen::mem::list<Token>, LexError> Lex();

  private:

    // Peek `n` number of characters in the file buffer.
    NG_INLINE char Peek(size_t n);

    // Get current character that the lexer is at.
    NG_AINLINE char Curr() const;

    // Execute an action if the next char is equal to `check`
    template<LAMBDA(void, void)>
    NG_INLINE void CheckNextAndThen(char check, Lambda then);

    // Return the valid token if the next character is '=' for an assignment
    // operator like '+=', '-=', etc.
    NG_INLINE Token GetIfNextIsAssignment(TokenKind kind, const char *src,
                                          size_t len);

    template<typename T>
    void AddToken(TokenKind, const char *,
                  TokenClassification, mem::list<Token>&);


    Allocator  *allocator;  // Token Allocator
    const File *file; // Source Buffer

    size_t line;
    size_t col;
  };


}} // namespace nextgen::jet



# endif //NEXTGEN_LEXER_H
