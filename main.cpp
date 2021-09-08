#include <nextgen/allocator.h>
#include <nextgen/jet/jet-diagnostics.h>
#include <nextgen/jet/jet-lexer.h>


using namespace nextgen::mem;
using namespace nextgen::jet;
using namespace nextgen;
int main(int argc, char **argv, char **envP) {

  auto lexer = nextgen::jet::Lexer<TokenMode>( R"("Hello" ace "There")",
                                      "src/test.jet", 19);

  auto tokens = lexer.lex();
  auto token = tokens[0];
  nextgen::Console::Log(token.name(), tokens[1].name(), tokens[2].name());
}