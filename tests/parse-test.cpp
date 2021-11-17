#include "../src/parser.h"

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
  auto decl = static_cast<const Syntax_Variable_Assignment*>(expr);
  ASSERT_EQ(expr->kind, Syntax_Kind::Variable_Assignment);
  //  //ASSERT_EQ(decl->name->name(), "hello"_intern);
  ASSERT_TRUE(::strncmp(decl->name->name().data(), "hello", decl->name->len()) == 0);
  ASSERT_EQ(decl->expression->kind, jet::Syntax_Kind::Literal_Value);
  ASSERT_EQ(CAST(const Syntax_Literal*, decl->expression)->literal->get_value<size_t>(), 23);
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
  auto decl = CAST(Syntax_Variable_Assignment*, node);
  auto list = CAST(Syntax_List*, decl->expression);


  ASSERT_EQ(decl->kind, Syntax_Kind::Variable_Assignment);
  ASSERT_EQ(CAST(const Syntax_Literal*, list->values[0])
  ->literal->get_value<size_t>
    (), 11);
  ASSERT_EQ(CAST(const Syntax_Literal*, list->values[1])
  ->literal->get_value<size_t>(), 2);
  ASSERT_EQ(CAST(const Syntax_Literal*, list->values[2])
  ->literal->get_value<size_t>
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
  auto decl = CAST(Syntax_Variable_Assignment*, expr);

  auto binary = CAST(Syntax_Binary*, decl->expression);

  auto lhs = CAST(Syntax_Literal*, binary->lhs);
  auto op  = binary->operation;
  auto rhs = CAST(Syntax_Binary*, binary->rhs);

  auto rhs_lhs = CAST(Syntax_Literal*, rhs->lhs);
  auto rhs_rhs = CAST(Syntax_Literal*, rhs->rhs);

  ASSERT_EQ(expr->kind, jet::Syntax_Kind::Variable_Assignment);
  //ASSERT_EQ(decl->name->name(), "wow"_intern);
  ASSERT_TRUE(::strncmp(decl->name->name().data(), "wow", decl->name->len()) == 0);
  ASSERT_EQ(binary->kind, jet::Syntax_Kind::Binary);

  ASSERT_EQ(op, Syntax_Binary_Op::Addition);
  ASSERT_EQ(lhs->literal->get_value<size_t>(), 2);
  ASSERT_EQ(rhs_lhs->literal->get_value<size_t>(), 3);
  ASSERT_EQ(rhs_rhs->literal->get_value<size_t>(), 2);
}

TEST(ParseTest, VariableDeclParenBinary) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  auto buf = "awesome := (2 + 3) * 2;";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  auto name = parser.curr(); parser.skip(2);
  auto expr = parser.parse_variable_assignment(name);
  auto decl = CAST(Syntax_Variable_Assignment*, expr);

  auto binary = CAST(Syntax_Binary*, decl->expression);

  auto lhs = CAST(Syntax_Binary*, binary->lhs);
  auto lhs_lhs = CAST(Syntax_Literal*, lhs->lhs);
  auto lhs_rhs = CAST(Syntax_Literal*, lhs->rhs);

  auto op  = binary->operation;
  auto rhs = CAST(Syntax_Literal*, binary->rhs);

  ASSERT_EQ(expr->kind, jet::Syntax_Kind::Variable_Assignment);
  //ASSERT_EQ(decl->name->name(), "wow"_intern);
  ASSERT_TRUE(::strncmp(decl->name->name().data(), "awesome", decl->name->len()) == 0);
  ASSERT_EQ(binary->kind, jet::Syntax_Kind::Binary);

  ASSERT_EQ(op, Syntax_Binary_Op::Multiplication);
  ASSERT_EQ(lhs_lhs->literal->get_value<size_t>(), 2);
  ASSERT_EQ(lhs_rhs->literal->get_value<size_t>(), 3);
  ASSERT_EQ(rhs->literal->get_value<size_t>(), 2);
}

TEST(ParseTest, VariableValueAssignment) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  auto buf = "weather <<= 2223;";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  auto node = (Syntax_Variable_Value_Assignment*) parser.parse_stmt();

  ASSERT_TRUE(::strncmp(node->name->name().data(), "weather", node->name->len()) == 0);
  ASSERT_EQ(node->op, Syntax_Assignment_Op::LShiftAssign);
  ASSERT_EQ(((Syntax_Literal * )(node->expression))->literal->get_value<size_t>(),
    2223);
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
  auto stmt = (Syntax_If*) node;

  auto if_cond_node = stmt->condition;
  auto if_cond      = (Syntax_Literal*) stmt->condition;

  auto variable_decl = (Syntax_Variable_Assignment*) stmt->body.statements[0];


  ASSERT_EQ(if_cond_node->kind, Syntax_Kind::Literal_Value);
  ASSERT_TRUE(::strncmp(if_cond->literal->name().data(), "value",
                        if_cond->literal->len()) == 0);

  ASSERT_TRUE(::strncmp(variable_decl->name->name().data(), "nice", variable_decl->name->len()) == 0);
  ASSERT_EQ(CAST(Syntax_Literal * , variable_decl->expression)->literal->get_value<size_t>(), 23);

}

TEST(ParseTest, WhileLoop) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto buf = "while some_value > 223 { another := 43; }";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);
  parser.skip(1);

  auto node = parser.parse_while();
  auto stmt = (Syntax_While*) node;

  auto while_cond_node = stmt->condition;

  auto while_cond      = (Syntax_Binary*) stmt->condition;
  auto lhs_cond = (Syntax_Literal*) while_cond->lhs;
  auto bin_op   = while_cond->operation;
  auto rhs_cond = (Syntax_Literal*) while_cond->rhs;

  auto variable_decl = (Syntax_Variable_Assignment*) stmt->body.statements[0];


  ASSERT_EQ(while_cond_node->kind, Syntax_Kind::Binary);
  ASSERT_TRUE(::strncmp(lhs_cond->literal->name().data(), "some_value",
                        lhs_cond->literal->len()) == 0);
  ASSERT_EQ(bin_op, Syntax_Binary_Op::Greater);
  ASSERT_EQ(rhs_cond->literal->get_value<size_t>(), 223);

  ASSERT_TRUE(::strncmp(variable_decl->name->name().data(), "another",
                        variable_decl->name->len()) == 0);
  ASSERT_EQ(CAST(Syntax_Literal * , variable_decl->expression)
              ->literal->get_value<size_t>(), 43);
}

TEST(ParseTest, ForStatement) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto buf = "for value in list_of_values { \n"
             " do_something := 23.233;"
             "}";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);

  auto parser = jet::Parser(&lexer);
  parser.skip(1);

  auto node = parser.parse_for();
  auto stmt = (Syntax_ForList*) node;
  auto variable_decl = (Syntax_Variable_Assignment*) stmt->body.statements[0];


  ASSERT_TRUE(::strncmp(stmt->list_var->name().data(), "value",
                        stmt->list_var->len()) == 0);
  ASSERT_TRUE(::strncmp(stmt->list_name->name().data(), "list_of_values",
                        stmt->list_name->len()) == 0);
  ASSERT_TRUE(::strncmp(variable_decl->name->name().data(), "do_something",
                        variable_decl->name->len()) == 0);
  ASSERT_EQ(CAST(Syntax_Literal * , variable_decl->expression)
              ->literal->get_value<double>(), 23.233);
}

TEST(ParseTest, SyntaxSemiColonError) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  auto buf = "// this is a comment\ncool := 23 / (22+3);\nanother_var := "
             "232 {";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  auto name = parser.curr(); parser.skip(2);
  parser.parse_variable_assignment(name);

  try {
    name = parser.curr(); parser.skip(2);
    parser.parse_variable_assignment(name);
  } catch(std::exception&) {}
}

TEST(ParseTest, SyntaxMissingDelimError) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  auto buf = "some_value := 22 * 2 / (232 + 4 + (2 + (3*2) / 2)";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  try {
    auto name = parser.curr(); parser.skip(2);
    parser.parse_variable_assignment(name);
  } catch (std::exception&) {}
}

TEST(ParseTest, SyntaxInvalidGlobalDecl) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  auto buf = "some_value := 2 + 3 / 232;\n value << 23;";
  auto buf_len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", buf_len);
  auto parser = jet::Parser(&lexer);

  try {
    auto name = parser.curr();
    parser.skip(2);
    parser.parse_variable_assignment(name);
    parser.parse();
  } catch (std::exception&) {}
}


TEST_SUITE_MAIN(ParseTest) {
  TEST_CALL(ParseTest, VariableDeclLiteral);
  TEST_CALL(ParseTest, VariableArrayDecl);
  TEST_CALL(ParseTest, VariableDeclBinary);
  TEST_CALL(ParseTest, VariableDeclParenBinary);
  TEST_CALL(ParseTest, VariableValueAssignment);
  TEST_CALL(ParseTest, IfStatement);
  TEST_CALL(ParseTest, WhileLoop);
  TEST_CALL(ParseTest, SyntaxSemiColonError);
  TEST_CALL(ParseTest, SyntaxMissingDelimError);
  TEST_CALL(ParseTest, SyntaxInvalidGlobalDecl);
  TEST_CALL(ParseTest, ForStatement);
}
