#include <gtest/gtest.h>
#include <nextgen/jet/Parse/Parser.h>

TEST(ParseTest, VariableDecl) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Buf = "var hello = 23";
  auto BufLen = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", BufLen);
  auto Parser = jet::Parser(&Lexer);


  Parser.Skip(1);
  auto Expr = Parser.ParseVariableDecl();
  auto Decl = Expr->VariableAssignment;

  ASSERT_EQ(Expr->Kind, jet::SyntaxKind::VariableAssignment);
  Console::Log(Decl.Name->Name());
  ASSERT_EQ(Decl.Name->Name(), "hello"_intern);
}

