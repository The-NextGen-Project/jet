#include <nextgen/jet/Lex/Lexer.h>
#include <nextgen/jet/Diagnostics.h>
typedef int integer_type;
using namespace nextgen;
using namespace nextgen::core;




int main(
  [[maybe_unused]] integer_type argc,
  [[maybe_unused]] char **argv,
  [[maybe_unused]] char **envP) {


  try {

    using namespace nextgen;
    using namespace nextgen::core;

/*
    Console::Log(Colors::RED, "Error "
                              "---------------------------------------------------------- src/test"
                              ".jet\n");
    Console::Log(Colors::RESET, "Reached unexpected end of file when lexing "
                               "token\n\n");
    Console::Log(Colors::WHITE, "50 |\t ", Colors::RED, "var", Colors::YELLOW,
                 " a ",
                 Colors::RESET, "= ",
                 Colors::GREEN, "\"Hello, World!\"\n");
    Console::Log(Colors::WHITE, "51 |\t ", Colors::RED, "var", Colors::YELLOW,
                 " variable ",
                 Colors::RESET, "= ",
                 Colors::BLUE, "0x\n");
    Console::Log("\t\t\t\t\t\t   ", Colors::RED, "~ <-- Token Ended "
                                                 "Unexpectedly\n");
    Console::Log(Colors::BLUE, "   = ", Colors::GREEN, "hint: ",
                 Colors::RESET, "You have an incomplete token written, check "
                                "the docs at ", Colors::YELLOW, "jetlang"
                                                                ".com/docs/tokens",
                                Colors::RESET,
                                " for "
                                "valid tokens.\n");
*/



    Arena<2> arena;

    auto buf = "var test = 0xffffffffffffffffffaaaaaaa test";
    auto len = strlen(buf);
    auto lexer = jet::Lexer::New (arena.begin,
                                  buf,
                                  len);
    lexer.NextToken();
    lexer.NextToken();
    lexer.NextToken();
    auto Error = lexer.NextToken().Error().Unwrap();
    jet::Diagnostic diagnostic = jet::Diagnostic(arena.begin, buf, len,
                                                 "src/test"
                                                                 ".jet");
    if (Error.Error == jet::LexErrorType::IntegerOverflow) {
      diagnostic.Build(Error);
    }

  } catch (std::exception &) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}