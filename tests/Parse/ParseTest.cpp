#include <gtest/gtest.h>
#include <nextgen/jet/Parse/Parser.h>

TEST(ParseTest, VariableDeclLiteral) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto buf = "var hello = 23";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer(arena.begin, buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  parser.skip(1);
  auto expr = parser.parse_variable_decl();
  auto decl = expr->VariableAssignment;
  ASSERT_EQ(expr->kind, jet::SyntaxKind::VariableAssignment);
  ASSERT_EQ(decl.name->name(), "hello"_intern);
  ASSERT_EQ(decl.expression->kind, jet::SyntaxKind::LiteralValue);
  ASSERT_EQ(decl.expression->Literal.literal->getValue<size_t>(), 23);
}

TEST(ParseTest, VariableDeclBinary) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto buf = "var wow = 2 + 3 * 2";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer(arena.begin, buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  parser.skip(1);
  auto expr = parser.parse_variable_decl();
  auto decl = expr->VariableAssignment;

  ASSERT_EQ(expr->kind, jet::SyntaxKind::VariableAssignment);

  ASSERT_EQ(decl.name->name(), "wow"_intern);
  ASSERT_EQ(decl.expression->kind, jet::SyntaxKind::Binary);
  ASSERT_EQ(decl.expression->Binary.operation, jet::SyntaxBinaryOp::Addition);
  ASSERT_EQ(decl.expression->Binary.lhs->Literal.literal->getValue<size_t>(),
    2);
  ASSERT_EQ(decl.expression->Binary.rhs->Binary.lhs->Literal
  .literal->getValue<size_t>(), 3);
  ASSERT_EQ(decl.expression->Binary.rhs->Binary.rhs->Literal
              .literal->getValue<size_t>(), 2);
}




