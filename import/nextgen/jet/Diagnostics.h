# ifndef JET_DIAGNOSTICS_H
# define JET_DIAGNOSTICS_H
# include "nextgen/Support/IO.h"
# include "nextgen/Support/Allocator.h"
# include "nextgen/Support/String.h"
# include "nextgen/Support/Panic.h"

namespace nextgen { namespace jet {

  // Lex Errors
  struct LexError;
  class Lexer;

  // Parsing Errors
  struct ParseError;
  class Parser;

  class Diagnostic {
  private:
    const char *FileBuffer;
    const char *FileName;
    str SourceLine;
    ArenaSegment *Memory;
    size_t BufferLength;
  public:
    Diagnostic() = default;

    Diagnostic(ArenaSegment *Mem, const char *Buffer, size_t BufLen, const
    char *FileName)
      : FileBuffer(Buffer), FileName(FileName), Memory(Mem), BufferLength(BufLen) {}
      
    /// Build Error Message for a Lexing Error
    void build(LexError Error);

    /// Build Error Message for a Lexing Error
    void build(ParseError Error);

  private:
    // Helpers
    str GetNthLineOfBuffer(size_t nth);
    void ErrorLexSetup(std::string &line, const char *message, LexError &error);

    // Lex Errors
    void ErrorIntegerOverflow(LexError &Error);
    void ErrorDigitOutOfRange(LexError &error);
    void ErrorInvalidChar(LexError &Error);
    void ErrorMalformedUTF8(LexError &Error);
    void ErrorMissingClosingDelim(LexError &error);
    void ErrorHexEscapeOutOfRange(LexError &error);
    void ErrorInvalidStringEscape(LexError &error);
  };

}}




# endif //JET_DIAGNOSTICS_H
