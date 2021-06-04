#include <nextgen/jet/lex/lexer.h>

typedef int integer_type;
using namespace nextgen;
using namespace nextgen::core;


void doSomething(Vec<int> a) {

}


int main(
  [[maybe_unused]] integer_type argc,
  [[maybe_unused]] char **argv,
  [[maybe_unused]] char **envP) {


  try {

    using namespace nextgen;
    using namespace nextgen::core;

    str val = "Hello";

    str first = StringInterner::Intern(val);

    auto range = (str) Range<const char *>(val, val + 5);
    range.setHash(first.getHashCache());
    str first2 = *StringInterner::Strings.find(range);

    Console::Log("CK ", first, "\nCK2 ", first2, "\n");
    printf("%p\n%p\n", first.begin(), first2.begin());

  } catch (std::exception &) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}