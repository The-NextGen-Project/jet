#include <nextgen/allocator.h>
#include <nextgen/jet/jet-diagnostics.h>
#include <nextgen/jet/jet-lexer.h>


using namespace nextgen::mem;
using namespace nextgen::jet;
using namespace nextgen;



struct MyType {
  int a;
  int b;
  std::string v;
  MyType(int a, int b, const std::string&v) : a(a), b(b), v(v) {}
};


int main(int argc, char **argv, char **envP) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode>( R"("Hello")",
                                      "src/test.jet", 7);

  auto tokens = lexer.lex();
  auto token = tokens[0];

  int a = 23;

  ASSERT_TRUE(::strncmp(token->name().begin(), "Hello", token->len()) == 0);
  ASSERT_EQ(token->getKind(), jet::TokenKind::String);

//  token = tokens[1];
//  ASSERT_TRUE(::strncmp(token->name().begin(), "ace", token->len()) == 0);
//  ASSERT_EQ(token->getKind(), jet::TokenKind::Identifier);
//
//  token = tokens[2];
//  ASSERT_TRUE(::strncmp(token->name().begin(), "There", token->len()) == 0);
//  ASSERT_EQ(token->getKind(), jet::TokenKind::String);
}