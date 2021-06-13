# ifndef JET_DIAGNOSTICS_H
# define JET_DIAGNOSTICS_H
# include "Lex/Lexer.h"
# include "nextgen/Support/IO.h"

namespace nextgen { namespace jet {

    class Diagnostic {
    private:
      const char *FileBuffer;
      const char *FileName;
      ArenaSegment *Memory;
      size_t BufferLength;
    public:
      Diagnostic(ArenaSegment *Mem, const char *Buffer, size_t BufLen, const
      char *FileName)
      : FileBuffer(Buffer), FileName(FileName), Memory(Mem), BufferLength(BufLen) {}

      void Build(LexError Error) {
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
            break;
          case DoubleUnderscore:
            break;
          case MissingClosingForChar:
            break;
          case MissingClosingForString:
            break;
          case InvalidDot:
            break;
          case InvalidNumberValue:
            break;
        }
      }

    private:
      // Helpers
      str GetNthLineOfBuffer(size_t Nth);


      // Lex Errors
      void ErrorIntegerOverflow(LexError &Error);
      void ErrorInvalidChar(LexError &Error);
      void ErrorMalformedUTF8(LexError &Error);
      void ErrorMissingClosingDelim(LexError &Error);
    };


  }}



# endif //JET_DIAGNOSTICS_H
