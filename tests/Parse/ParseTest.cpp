#include <gtest/gtest.h>
#include <nextgen/jet/Parse/Parser.h>

TEST(ParseTest, VariableDeclLiteral) {
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
  ASSERT_EQ(Decl.Name->name(), "hello"_intern);
  ASSERT_EQ(Decl.Expression->Kind, jet::SyntaxKind::LiteralValue);
  ASSERT_EQ(Decl.Expression->Literal.Literal->getValue<size_t>(), 23);
}

TEST(ParseTest, VariableDeclBinary) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Buf = "var wow = 2 * 3";
  auto BufLen = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", BufLen);
  auto Parser = jet::Parser(&Lexer);

  Parser.Skip(1);
  auto Expr = Parser.ParseVariableDecl();
  auto Decl = Expr->VariableAssignment;

  ASSERT_EQ(Expr->Kind, jet::SyntaxKind::VariableAssignment);
  ASSERT_EQ(Decl.Name->name(), "wow"_intern);
  ASSERT_EQ(Decl.Expression->Kind, jet::SyntaxKind::Binary);
  ASSERT_EQ(Decl.Expression->Binary.Operation, jet::SyntaxBinaryOp::Multiplication);
  ASSERT_EQ(Decl.Expression->Binary.LHS->Literal.Literal->getValue<size_t>(),
    2);
  ASSERT_EQ(Decl.Expression->Binary.RHS->Literal.Literal->getValue<size_t>(),
            3);
}

