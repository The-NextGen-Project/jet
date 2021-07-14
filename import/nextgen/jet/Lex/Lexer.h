# ifndef NEXTGEN_LEXER_H
# define NEXTGEN_LEXER_H

# include "Token.h"
# include "../Diagnostics.h"

namespace nextgen { namespace jet { using namespace nextgen::core;

  enum LexErrorType {

    // Other
    UnexpectedEOF,
    OutOfRange,
    Unreachable,

    // Strings
    MalformedUTF8,
    InvalidChar,
    InvalidStringEscape,
    InvalidUnicodeEscapeClose,
    HexEscapeOutOfRange,
    MissingClosingDelim,

    // Numbers
    DigitOutOfRange,
    InvalidDot,
    InvalidNumberValue,
    IntegerOverflow,
    FloatingPointOverflow,

  };

  enum LexMode {
    TokenMode,
    PrintingMode
  };


  struct LexError {
    LexErrorType Error;
    TokenTraits::SourceLocation Location;
    Token FailedToken;

    int Metadata[5];
  };

  class Lexer {
    using Allocator = mem::ArenaSegment;
    using File = char;

    Allocator   *Memory;
    const File  *Buffer;
    const size_t BufferSize;

    size_t CurrentLine    = 1;
    size_t CurrentColumn  = 1;
    size_t BufferPosition = 0;

    LexMode Mode;

    Diagnostic ErrorBuilder;

    bool Fatal = false; // Error Thrown

  public:

    friend class Parser;

    Lexer(Allocator *Mem, const File *Buffer, const char *FileName, 
                                              const size_t BufSize,
                                              LexMode Mode = LexMode::TokenMode)
    : Memory(Mem), Buffer(Buffer), BufferSize(BufSize), Mode(Mode) {
      ErrorBuilder = Diagnostic(Mem, Buffer, BufSize, FileName);
    }

    /// Get the next valid Token in the File stream
    auto NextToken() -> Token;

    /// Print the values of token with Syntax Highlighting
    void PrintNextToken();

    /// Lex all tokens. If an error is encountered, report it
    /// and count it as fatal. Compiler cannot proceed without
    /// lexing phase being valid.
    NG_INLINE auto Lex() -> Vec<Token> {
      auto Tokens = Vec<Token>{};
      do {
        auto Instance = NextToken();
        Tokens.push(Instance);
      } while (!Fatal && BufferPosition < BufferSize);

      // Insert EOFToken for Out of Range Values
      Tokens.push(Token("\n", {CurrentLine, CurrentColumn},
                        TokenKind::EOFToken));
      return Tokens;
    }

    /// Print the tokens using syntax highlighting with the given
    /// buffer. Used for Re-lexing in error reporting.
    NG_INLINE void LexPrint() noexcept {
      do {
        PrintNextToken();
      } while (BufferPosition < BufferSize && Buffer);
    }


    /// Peek `NChars` characters in the file buffer.
    NG_INLINE char Peek(size_t NChars)  {
      return *(Buffer + NChars);
    }

    /// Get the latest character in the file buffer
    NG_AINLINE char Curr()  {
      return *Buffer;
    }

    /// Move the lexer forward by `NChars` and fill in `Error` for
    /// unexpected EOF or Out of Range Error that may be encountered.
    NG_INLINE auto Next(size_t NChars) -> char {

      if (BufferPosition >= BufferSize) {
        Fatal = true;
        return '\0';
      }

      CurrentColumn += NChars;
      BufferPosition += NChars;
      Buffer += NChars;
      return *Buffer;
    }

  private:

    /// Matches a new line encountered in the buffer and handles
    /// them appropriately depending on \n or \r.
    NG_INLINE void SkipNewLine() {
      CurrentLine++;
      CurrentColumn = 1;

      Next(1);

      char Current = *Buffer;
      char NextCh  = Peek(1);

      if (NextCh != Current && (NextCh == '\n' || NextCh == '\r')) {
        if (Mode == LexMode::PrintingMode) Console::Log(NextCh);
        ++CurrentLine, Next(1);
      }
    }

  };


}} // namespace nextgen::jet



# endif //NEXTGEN_LEXER_H
