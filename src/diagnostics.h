#ifndef JET_DIAGNOSTICS_H
#define JET_DIAGNOSTICS_H

#include "util/io.h"
#include "allocator.h"
#include "util/str.h"
#include "util/panic.h"
#include "types/error-types.h"

namespace nextgen { namespace jet {

  class Diagnostic {
    const char *file_buf;
    const char *file_name;
    size_t      buf_len;
    std::string_view source_line;
  public: // Constructors
    Diagnostic() = default;
    Diagnostic(const char *buffer, size_t buf_len, const char *file_name)
      : file_buf(buffer), file_name(file_name), buf_len(buf_len) {}

  public: // Impl
    void build(Lex_Error error);
    void build(Parse_Error error);
    auto send_exception(void) const {
      Console::Log(Colors::RED, "error: ", Colors::RESET, "Aborting due to previous errors\n");
      throw std::exception();
    }
  private: // Impl
    // Helpers
    void ErrorLexSetup(std::string &line, const char *message, Lex_Error &error);

    template<bool POINT = false>
      void ErrorParseSetup(size_t const ln,
                           const char *message,
                           Token const *reported_token,
                           Source_Location loc);

    // Lex Errors (We will keep these PascalCase)
    void ErrorIntegerOverflow(Lex_Error &error);
    void ErrorDigitOutOfRange(Lex_Error &error);
    void ErrorInvalidChar(Lex_Error &error);
    void ErrorMalformedUTF8(Lex_Error &error);
    void ErrorMissingClosingDelim(Lex_Error &error);
    void ErrorHexEscapeOutOfRange(Lex_Error &error);
    void ErrorInvalidStringEscape(Lex_Error &error);

    // Parse Errors (We will keep these PascalCase)
    void ErrorParseExpectedToken(Parse_Error const &error);
    void ErrorParseMissingClosingDelim(Parse_Error const &error);
    void ErrorParseInvalidTokenAfterIdentInGlobalScope(Parse_Error const &error);
  };

}}




#endif //JET_DIAGNOSTICS_H
