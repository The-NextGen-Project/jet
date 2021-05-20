#include <nextgen/support/core.h>
#include <nextgen/support/panic.h>
#include <nextgen/support/str.h>
#include <nextgen/support/allocator.h>


using namespace nextgen;
using namespace nextgen::core;

typedef int integer_type;


int main(
  [[maybe_unused]] integer_type argc,
  [[maybe_unused]] char **argv,
  [[maybe_unused]] char **envP) {

  // This is all C++
  //std::ios::sync_with_stdio(false);

  try {

    Console::Log(Colors::GREEN, (int)'\\', "\n", Colors::BLUE, (int)'_',
                 "\n");

  } catch (std::exception &) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}