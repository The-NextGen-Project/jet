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


  // Printing Mode re-analyzes the file, while printing out tokens with
  // syntax-highlighting to the console. This allows for pretty-error messages
  // with colored output.
  enum LexMode {
    TokenMode,
    PrintingMode
  };


  struct LexError {

    // The type of error
    LexErrorType error;

    // Error location
    TokenTraits::SourceLocation location;

    // Token value lexed until error
    Token failed_token;

    // Error metadata
    int metadata[5];
  };


  // TODO: Make this a templated class
  class Lexer {
    using Allocator = mem::ArenaSegment;
    using File = char;

    Allocator   *allocator;
    const File  *buffer;
    const size_t buf_size;

    size_t line    = 1;
    size_t column  = 1;
    size_t buf_pos = 0;

    LexMode Mode;

    Diagnostic diagnostics;

    bool fatal = false; // Error Thrown

  public:

    friend class Parser;

    Lexer(Allocator *mem, const File *buffer, const char *file_name,
          const size_t buffer_size,
          LexMode Mode = LexMode::TokenMode)
    : allocator(mem), buffer(buffer), buf_size(buffer_size), Mode(Mode) {

      diagnostics = Diagnostic(mem, buffer, buffer_size, file_name);
    }

    /// Get the next valid Token in the File stream
    auto next_token() -> Token;

    /// Print the values of token with Syntax Highlighting
    void PrintNextToken();

    /// Lex all tokens from the file buffer, and if fatal is checked,
    /// it means that an error has been thrown, and will exit the program.
    auto lex() -> Vec<Token> {
      auto tokens = Vec<Token>{};
      do {
        auto instance = next_token();
        tokens.push(instance);
      } while (!fatal && buf_pos < buf_size);

      // Insert EOFToken for Out of Range values
      tokens.push(Token("\n", {line, column},
                        TokenKind::EOFToken));
      return tokens;
    }

    /// Print the tokens using syntax highlighting with the given
    /// buffer. Used for Re-lexing in error reporting.
   void LexPrint() noexcept {
      do {
        PrintNextToken();
      } while (buf_pos < buf_size && buffer);
    }


    /// Peek `NChars` characters in the file buffer.
    char Peek(size_t NChars)  {
      return *(buffer + NChars);
    }

    /// Get the latest character in the file buffer
    char Curr()  {
      return *buffer;
    }

    /// Move the lexer forward by `NChars` and fill in `Error` for
    /// unexpected EOF or Out of Range Error that may be encountered.
    char Next(size_t NChars) {

      if (buf_pos >= buf_size) {
        fatal = true;
        return '\0';
      }

      column += NChars;
      buf_pos += NChars;
      buffer += NChars;
      return *buffer;
    }

  private:

    /// Matches a new line encountered in the buffer and handles
    /// them appropriately depending on \n or \r.
    void SkipNewLine() {
      line++;
      column = 1;

      Next(1);

      char Current = *buffer;
      char NextCh  = Peek(1);

      if (NextCh != Current && (NextCh == '\n' || NextCh == '\r')) {
        if (Mode == LexMode::PrintingMode) Console::Log(NextCh);
        ++line, Next(1);
      }
    }

  };


}} // namespace nextgen::jet



# endif //NEXTGEN_LEXER_H
