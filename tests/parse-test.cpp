#include <nextgen/jet/jet-parser.h>

#ifndef TEST
  #define TEST(suite, name) void suite##_##name##_##test()
#endif
#ifndef TEST_SUITE_MAIN
  #define TEST_SUITE_MAIN(suite) void main_##suite()
#endif
#ifndef TEST_CALL
  #define TEST_CALL(suite, name) suite##_##name##_##test()
#endif

TEST(ParseTest, VariableDeclLiteral) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  auto buf = "hello := 23;";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  auto name = parser.curr(); parser.skip(2);
  auto expr = parser.parse_variable_assignment(name);
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


  auto buf = "my_list := [11, 2, 55];";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  auto name = parser.curr(); parser.skip(2);
  auto node = parser.parse_variable_assignment(name);
  auto decl = CAST(SyntaxVariableAssignment*, node);
  auto list = CAST(SyntaxList*, decl->expression);


  ASSERT_EQ(decl->kind, SyntaxKind::VariableAssignment);
  ASSERT_EQ(CAST(SyntaxLiteral*, list->values[0])->literal->getValue<size_t>
  (), 11);
  ASSERT_EQ(CAST(SyntaxLiteral*, list->values[1])->literal->getValue<size_t>(), 2);
  ASSERT_EQ(CAST(SyntaxLiteral*, list->values[2])->literal->getValue<size_t>
  (), 55);
}

TEST(ParseTest, VariableDeclBinary) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto buf = "wow := 2 + 3 * 2;";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  auto name = parser.curr(); parser.skip(2);
  auto expr = parser.parse_variable_assignment(name);
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


  auto buf = "if value { nice := 23; }";
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

}

TEST(ParseTest, WhileLoop) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto buf = "while some_value > 2 { another := 23; }";
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
}

TEST_SUITE_MAIN(ParseTest) {
  TEST_CALL(ParseTest, VariableDeclLiteral);
  TEST_CALL(ParseTest, VariableArrayDecl);
  TEST_CALL(ParseTest, VariableDeclBinary);
  TEST_CALL(ParseTest, IfStatement);
}
