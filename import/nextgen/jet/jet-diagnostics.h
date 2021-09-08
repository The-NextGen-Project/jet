# ifndef JET_JET_DIAGNOSTICS_H
# define JET_JET_DIAGNOSTICS_H
# include "nextgen/io.h"
# include "nextgen/allocator.h"
# include "nextgen/str.h"
# include "nextgen/panic.h"

namespace nextgen { namespace jet {

  // Lex Errors
  struct LexError;

  // Printing Mode re-analyzes the file, while printing out tokens with
  // syntax-highlighting to the console. This allows for pretty-error messages
  // with colored output.
  enum LexMode {
    TokenMode,
    PrintingMode
  };

  template<LexMode Mode>
  class Lexer;

  // Parsing Errors
  struct ParseError;
  class Parser;

  class Diagnostic {
  private:
    const char *file_buf;
    const char *file_name;
    size_t      buf_len;

    // Line where error has occurred
    str source_line;

  public:
    Diagnostic() = default;

    Diagnostic(const char *buffer, size_t buf_len, const
    char *file_name)
      : file_buf(buffer), file_name(file_name), buf_len(buf_len) {}
      
    /// Build error Message for a Lexing error
    void build(LexError error);

    /// Build error Message for a Lexing error
    void build(ParseError error);

    void send_exception() {
      Console::Log(Colors::RED, "error: ", Colors::RESET, "Aborting due to "
                                                          "previous errors");
      throw std::exception();
    }
  private:
    // Helpers
    void ErrorLexSetup(std::string &line, const char *message, LexError &error);

    // Lex Errors (We will keep these PascalCase)
    void ErrorIntegerOverflow(LexError &error);
    void ErrorDigitOutOfRange(LexError &error);
    void ErrorInvalidChar(LexError &error);
    void ErrorMalformedUTF8(LexError &error);
    void ErrorMissingClosingDelim(LexError &error);
    void ErrorHexEscapeOutOfRange(LexError &error);
    void ErrorInvalidStringEscape(LexError &error);
  };

}}




# endif //JET_JET_DIAGNOSTICS_H
