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
    void Build(LexError Error);

    /// Build Error Message for a Lexing Error
    void Build(ParseError Error);

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

}}




# endif //JET_DIAGNOSTICS_H
