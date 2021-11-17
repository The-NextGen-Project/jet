#ifndef NEXTGEN_LEXER_H
#define NEXTGEN_LEXER_H

#include "types/lexer-types.h"
#include "diagnostics.h"
#include "allocator.h"

namespace nextgen { namespace jet { using namespace nextgen::core;


  // Lexical analysis has two different modes for two purposes. Depending on
  // the case, the lexer will either register tokens or it will simply print
  // tokens to the terminal.
  //
  // TokenMode: Specifies the mode in which tokens are lexed and output is
  // returned.
  template<Lex_Mode Mode = TokenMode>
  class Lexer {
    using File = char;

    const File  *buffer;
    const size_t buf_size;

    size_t line    = 1;
    size_t column  = 1;
    size_t buf_pos = 0;

    Diagnostic diagnostics;
    Range<Token*> tokens;


    bool fatal = false;

    // Ensure that branches are taken away during compile-time with guaranteed
    // compile-time value if statements. This is used so that both printing mode
    // and normal mode do not have to be separate functions.
    static constexpr bool OUTPUT_MODE = Mode == Lex_Mode::PrintingMode;
    static constexpr bool NORMAL_MODE = Mode == Lex_Mode::TokenMode;

  public: // Constructors
    friend class Parser;

    Lexer(const File *buffer, const char *file_name, const size_t buffer_size)
    : buffer(buffer), buf_size(buffer_size)
    {
      this->diagnostics = Diagnostic(buffer, buffer_size, file_name);
    }

  public: // Impl

    NG_AINLINE char peek(size_t chars)  {
      return *(buffer + chars);
    }

    NG_AINLINE char curr()  {
      return *buffer;
    }

    char next(size_t n) {

      if (this->buf_pos >= this->buf_size) {
        if (Mode == TokenMode) {
          this->diagnostics.build(Lex_Error {
            Lex_Error_Kind::Out_Of_Range,
            { line, column },
            Token("\0", {line, column}, Token_Kind::EOFToken)
          });
          UNREACHABLE;
        }  else this->fatal = true;
      }
      this->column += n;
      this->buf_pos += n;
      this->buffer += n;
      return *buffer;
    }

    auto lex() -> std::vector<Token>;
    void lex_int();
    template<int radix, int skip>
    void lex_int();
    void lex_float(int skip, int start=0);
    void lex_str();
    void lex_ident();

  private:

    /// Matches a new line encountered in the buffer and handles
    /// them appropriately depending on \n or \r.
    void skip_new_line() {
      line++;
      column = 1;

      next(1);

      char current = *buffer;
      char next_ch  = peek(1);

      if (next_ch not_eq current and (next_ch == '\n' or next_ch == '\r')) {
        if (Mode == Lex_Mode::PrintingMode)
          Console::Log(next_ch);
        ++line, column = 1, next(1);
      }
    }

  };


}} // namespace nextgen::jet



#endif //NEXTGEN_LEXER_H
