#include <nextgen/jet/Lex/Lexer.h>
#include <nextgen/Support/IO.h>
#include <nextgen/Support/Panic.h>
#include <iostream>
#include <iomanip>
#include <memory>


typedef int integer_type;

using namespace nextgen;
using namespace nextgen::core;
using namespace nextgen::jet;



int main(integer_type argc, char **argv, char **envP) {

  try {

    Vec<int> wow;
    wow.push(123);


  } catch (std::exception &) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}