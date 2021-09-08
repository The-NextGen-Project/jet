#include <gtest/gtest.h>
#include <nextgen/jet/jet-parser.h>

TEST(ParseTest, VariableDeclLiteral) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;
  
  auto buf = "var hello = 23";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  parser.skip(1);
  auto expr = parser.parse_variable_decl();
  auto decl = static_cast<SyntaxVariableAssignment*>(expr);
  ASSERT_EQ(expr->kind, SyntaxKind::VariableAssignment);
//  //ASSERT_EQ(decl->name->name(), "hello"_intern);
  ASSERT_TRUE(::strncmp(decl->name->name().begin(), "hello", decl->name->len()) == 0);
  ASSERT_EQ(decl->expression->kind, jet::SyntaxKind::LiteralValue);
  ASSERT_EQ(CAST(SyntaxLiteral*, decl->expression)->literal->getValue<size_t>
    (), 23);

  arena::deinit();
}

TEST(ParseTest, VariableArrayDecl) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto buf = "var my_list = [1, 2, 55]";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  parser.skip(1);
  auto node = parser.parse_variable_decl();
  auto decl = CAST(SyntaxVariableAssignment*, node);
  auto list = CAST(SyntaxList*, decl->expression);

  ASSERT_EQ(decl->kind, SyntaxKind::VariableAssignment);
  ASSERT_EQ(CAST(SyntaxLiteral*, list->values[0])->literal->getValue<size_t>(), 1);
  ASSERT_EQ(CAST(SyntaxLiteral*, list->values[1])->literal->getValue<size_t>(), 2);
  ASSERT_EQ(CAST(SyntaxLiteral*, list->values[2])->literal->getValue<size_t>(), 55);
}

TEST(ParseTest, VariableDeclBinary) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto buf = "var wow = 2 + 3 * 2";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  parser.skip(1);
  auto expr = parser.parse_variable_decl();
  auto decl = CAST(SyntaxVariableAssignment*, expr);

  auto binary = CAST(SyntaxBinary*, decl->expression);

  auto lhs = CAST(SyntaxLiteral*, binary->lhs);
  auto op  = binary->operation;
  auto rhs = CAST(SyntaxBinary*, binary->rhs);

  auto rhs_lhs = CAST(SyntaxLiteral*, rhs->lhs);
  auto rhs_rhs = CAST(SyntaxLiteral*, rhs->rhs);

  ASSERT_EQ(expr->kind, jet::SyntaxKind::VariableAssignment);
  //ASSERT_EQ(decl->name->name(), "wow"_intern);
  ASSERT_TRUE(::strncmp(decl->name->name().begin(), "wow", decl->name->len()) == 0);
  ASSERT_EQ(binary->kind, jet::SyntaxKind::Binary);

  ASSERT_EQ(op, SyntaxBinaryOp::Addition);
  ASSERT_EQ(lhs->literal->getValue<size_t>(),2);
  ASSERT_EQ(rhs_lhs->literal->getValue<size_t>(), 3);
  ASSERT_EQ(rhs_rhs->literal->getValue<size_t>(), 2);
}

TEST(ParseTest, IfStatement) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto buf = "if value { var nice = 23 }";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);
  parser.skip(1);


  auto node = parser.parse_if();
  auto stmt = (SyntaxIf*) node;

  auto if_cond_node = stmt->condition;
  auto if_cond      = (SyntaxLiteral*) stmt->condition;

  auto variable_decl = (SyntaxVariableAssignment*) stmt->body.statements[0];

  ASSERT_EQ(if_cond_node->kind, SyntaxKind::LiteralValue);
  ASSERT_TRUE(::strncmp(if_cond->literal->name().begin(), "value",
                        if_cond->literal->len()) == 0);
  ASSERT_TRUE(::strncmp(variable_decl->name->name().begin(), "nice", variable_decl->name->len()) == 0);
  ASSERT_EQ(CAST(SyntaxLiteral*, variable_decl->expression)->literal->getValue<size_t>(), 23);


  vec::clear(stmt->body.statements);
}

