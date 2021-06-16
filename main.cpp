#include <nextgen/jet/Lex/Lexer.h>
#include <nextgen/jet/Diagnostics.h>
typedef int integer_type;
using namespace nextgen;
using namespace nextgen::core;

template<typename A>
A add(A x, A y) {
  return x + y;
}


int main(integer_type argc, char **argv, char **envP) {


  try {

    using namespace nextgen;
    using namespace nextgen::core;
    using namespace nextgen;
    using namespace nextgen::core;

    auto z = 2e+23;

    Console::Log(z);

  } catch (std::exception &) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}