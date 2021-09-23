#include <nextgen/allocator.h>
#include <nextgen/jet/jet-syntax-nodes.h>


using namespace nextgen::mem;
using namespace nextgen::jet;
using namespace nextgen;



int main(int argc, char **argv, char **envP) {

  try {
    SyntaxNode *while_node = new SyntaxWhile(nullptr, SyntaxBlock{});
    ASSERT_EQ(while_node->kind, SyntaxKind::LiteralValue);
  } catch(std::exception&) {
    arena::deinit();
  }
}