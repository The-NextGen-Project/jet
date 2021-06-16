# ifndef NEXTGEN_LEXER_H
# define NEXTGEN_LEXER_H

# include "Token.h"

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

    class Diagnostic {
    private:
      const char *FileBuffer;
      const char *FileName;
      ArenaSegment *Memory;
      size_t BufferLength;
    public:
      Diagnostic() = default;

      Diagnostic(ArenaSegment *Mem, const char *Buffer, size_t BufLen, const
      char *FileName)
        : FileBuffer(Buffer), FileName(FileName), Memory(Mem), BufferLength(BufLen) {}

      /// Build Error Message for a Lexing Error
      NG_INLINE void Build(LexError Error) {
        Console::Log(Colors::RED, "Error "
                                  "---------------------------------------------------------- ", FileName, ":",
                     Error.Location.Line, ":", Error.Location
                       .Column, "\n",
                     Colors::RESET);
        switch (Error.Error) {
          case MalformedUTF8:
            break;
          case InvalidChar:
            break;
          case UnexpectedEOF:
            break;
          case OutOfRange:
            break;
          case Unreachable:
            break;
          case IntegerOverflow:
            ErrorIntegerOverflow(Error);
            break;
          case FloatingPointOverflow:
            break;
          case DigitOutOfRange:
            ErrorDigitOutOfRange(Error);
            break;
          case MissingClosingDelim:
            ErrorMissingClosingDelim(Error);
            break;
          case InvalidDot:
            break;
          case InvalidNumberValue:
            break;
          case InvalidStringEscape:
            ErrorInvalidStringEscape(Error);
            break;
          case InvalidUnicodeEscapeClose:
            break;
          case HexEscapeOutOfRange:
            ErrorHexEscapeOutOfRange(Error);
            break;
          default:
            UNREACHABLE;
        }
        Console::Log(Colors::RESET, "\n");
      }

    private:
      // Helpers
      str GetNthLineOfBuffer(size_t Nth);
      void ErrorLexSetup(std::string &Line, const char *Message, LexError &Error);

      template<typename ... Args> void AddHint
                      (std::string &Line, Args&& ...Hint);


      // Lex Errors
      void ErrorIntegerOverflow(LexError &Error);
      void ErrorDigitOutOfRange(LexError &Error);
      void ErrorInvalidChar(LexError &Error);
      void ErrorMalformedUTF8(LexError &Error);
      void ErrorMissingClosingDelim(LexError &Error);
      void ErrorHexEscapeOutOfRange(LexError &Error);
      void ErrorInvalidStringEscape(LexError &Error);
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


  public:

    /// Initialization of values through Lexer::New
    static auto New(Allocator *Mem, const File *Buffer, const char *FileName,
                    const size_t BufferLength,
                    LexMode Mode = LexMode::TokenMode) -> Lexer {
      Lexer Instance = Lexer(Mem, Buffer, BufferLength);
      Instance.Mode = Mode;
      Instance.ErrorBuilder = Diagnostic(Mem, Buffer, BufferLength, FileName);
      return Instance;
    }

    /// Get the next valid Token in the File stream
    auto NextToken() -> Result<Token, LexError>;

    /// Print the values of token with Syntax Highlighting
    void PrintNextToken();

    /// Lex all tokens. If an error is encountered, report it
    /// and count it as fatal. Compiler cannot proceed without
    /// lexing phase being valid.
    NG_INLINE auto Lex() -> Vec<Token> {
      Vec<Token> Tokens = Vec<Token>::New();
      do {
        auto Instance = NextToken();
        if (Instance.IsErr()) {
          ErrorBuilder.Build(Instance.Error().Unwrap());
        }
        Tokens.Add(Instance.Unwrap());
      } while (Buffer);
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
    NG_INLINE auto Peek(size_t NChars) -> char {
      return *(Buffer + NChars);
    }

    /// Get the latest character in the file buffer
    NG_AINLINE auto Curr() -> char const {
      return *Buffer;
    }

    /// Move the lexer forward by `NChars` and fill in `Error` for
    /// unexpected EOF or Out of Range Error that may be encountered.
    NG_INLINE auto Next(size_t NChars) -> char {

      if (BufferPosition >= BufferSize) {

        // TODO: Get Diagnostics Working in Panic Situation
        LexError Error = LexError {
          LexErrorType::OutOfRange,
          { CurrentLine, CurrentColumn }
        };

        return '\0';
      }

      CurrentColumn += NChars;
      BufferPosition += NChars;
      Buffer += NChars;
      return *Buffer;
    }

  private:

    /// Initializer Shadow Constructor
    Lexer(Allocator *Mem, const File *Buffer, const size_t BufSize)
    : Memory(Mem), Buffer(Buffer), BufferSize(BufSize) {}

    /// Matches a new line encountered in the buffer and handles
    /// them appropriately depending on \n or \r.
    NG_INLINE void SkipNewLine() {
      CurrentLine++;
      char Current = *Buffer;
      char NextCh  = Next(1);
      match(Current, NextCh) {
        group('\n', '\r'):
        group ('\r', '\n'): {
          if (Mode == LexMode::PrintingMode) { // Handle Print Case
            Console::Log(NextCh);
          }
          ++CurrentLine, Next(1);
        }
        default:
          break;
      }
    }

  };


}} // namespace nextgen::jet



# endif //NEXTGEN_LEXER_H
