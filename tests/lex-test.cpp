#include <nextgen/jet/jet-lexer.h>

#ifndef TEST
  #define TEST(suite, name) void suite##_##name##_##test()
#endif
#ifndef TEST_SUITE_MAIN
  #define TEST_SUITE_MAIN(suite) void main_##suite()
#endif
#ifndef TEST_CALL
  #define TEST_CALL(suite, name) suite##_##name##_##test()
#endif


TEST(LexTest, NonFMTNumbers) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode> ( "1211", "src/test.jet", 4);
  auto token = lexer.lex()[0];


//  ASSERT_EQ(token->getKind(), jet::TokenKind::Integer);
//  ASSERT_EQ(token->getValue<decltype(UINTPTR_MAX)>(), 1211);

}

TEST(LexTest, DecimalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode> ( "23.246", "src/test.jet", 6);
  auto token = lexer.lex()[0];

  ASSERT_EQ(token->template getValue<double>(), 23.246);
  ASSERT_EQ(token->getKind(), jet::TokenKind::Decimal);
}


TEST(LexTest, HexadecimalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode>( "0x1111", "src/test.jet", 6);
  auto token = lexer.lex()[0];
//  ASSERT_EQ(token->getValue<decltype(UINTPTR_MAX)>(), 0x1111);
//  ASSERT_EQ(token->getKind(), jet::TokenKind::Integer);
}


TEST(LexTest, BinaryNumber) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode>( "0b011101110", "src/test.jet", 11);
  auto token = lexer.lex()[0];
  ASSERT_EQ(token->getValue<decltype(UINTPTR_MAX)>(), 0b011101110);
  ASSERT_EQ(token->getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, OctalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode>( "0112022", "src/test.jet", 7);
  auto token = lexer.lex()[0];
  ASSERT_EQ(token->getValue<decltype(UINTPTR_MAX)>(), 0112022);
  ASSERT_EQ(token->getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Base36Number) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode>( "0#helloworld", "src/test.jet", 12);
  auto token = lexer.lex()[0];

  ASSERT_EQ(token->getValue<decltype(UINTPTR_MAX)>(), 1767707668033969);
  ASSERT_EQ(token->getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Identifier) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode>( "ident", "src/test.jet", 5);
  auto token = lexer.lex()[0];

  ASSERT_TRUE(::strncmp(token->name().begin(), "ident", 5) == 0);
  ASSERT_EQ(token->getKind(), jet::TokenKind::Identifier);
}

TEST(LexTest, Keyword) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode>( "while extern", "src/test.jet",
                                      12);

  auto tokens = lexer.lex();
  auto token = tokens[0];

  ASSERT_TRUE(::strncmp(token->name().begin(), "while", 5) == 0);
  ASSERT_EQ(token->getKind(), jet::TokenKind::KeywordWhile);


  token = tokens[1];
  ASSERT_TRUE(::strncmp(token->name().begin(), "extern", 6) == 0);
  ASSERT_EQ(token->getKind(), jet::TokenKind::KeywordExtern);
}

TEST(LexTest, String) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto lexer = jet::Lexer<TokenMode>( R"("Hello" ace "There")",
                                      "src/test.jet", 19);

  auto tokens = lexer.lex();
  auto token = tokens[0];

  ASSERT_TRUE(::strncmp(token->name().begin(), "Hello", token->len()) == 0);
  ASSERT_EQ(token->getKind(), jet::TokenKind::String);


  token = tokens[1];

  ASSERT_TRUE(::strncmp(token->name().begin(), "ace", token->len()) == 0);
  ASSERT_EQ(token->getKind(), jet::TokenKind::Identifier);


  token = tokens[2];

  ASSERT_TRUE(::strncmp(token->name().begin(), "There", token->len()) == 0);
  ASSERT_EQ(token->getKind(), jet::TokenKind::String);

}


TEST(LexTest, StringEscape) { // TODO: Add Unicode Escape Later

  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto buf = R"(var every_escape = "I have\x56 every \t thing\n that could \b\v in \r")";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  ArenaVec<Token> tokens{nullptr, nullptr};
  try {
    tokens = lexer.lex();
  } catch (std::exception &) {
    // Do nothing ...
    return;
  }
  auto token = tokens[3];
  Console::Log("Name: ", token->name(), '\n');

  auto compare = "I have\x56 every \t thing\n that could \b\v in \r";
  ASSERT_TRUE(::strncmp(token->name().begin(), compare, token->len()) == 0);
}

TEST(LexTest, AllTokens) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;



  auto buf = "+ += ++ - -= -- * *=  ** **= / /= > >= >> >>= < <= << <<= %  %="
             "() {} "
             "[] ~ ^ ^= "
             "| |= & &= = == : ; @ ! ? . .. ..."; // 43 tokens
             auto len = strlen(buf);
             auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);


             ArenaVec<Token> tokens{nullptr, nullptr};
             try {
               tokens = lexer.lex();
             } catch (std::exception &) {
               // Do nothing ...
               return;
             }
             ASSERT_EQ(tokens.len(), 45);
             ///*  ASSERT_EQ(tokens[0].getKind(), jet::TokenKind::Plus);
             //  ASSERT_EQ(tokens[1].getKind(), jet::TokenKind::PlusEquals);
             //  ASSERT_EQ(tokens[2].getKind(), jet::TokenKind::PlusPlus);
             //  ASSERT_EQ(tokens[3].getKind(), jet::TokenKind::Minus);
             //  ASSERT_EQ(tokens[4].getKind(), jet::TokenKind::MinusEquals);
             //  ASSERT_EQ(tokens[5].getKind(), jet::TokenKind::MinusMinus);
             //  ASSERT_EQ(tokens[6].getKind(), jet::TokenKind::Star);
             //  ASSERT_EQ(tokens[7].getKind(), jet::TokenKind::MulEquals);
             //  ASSERT_EQ(tokens[8].getKind(), jet::TokenKind::Pow);
             //  ASSERT_EQ(tokens[9].getKind(), jet::TokenKind::Slash);*//*


}

TEST(LexTest, ErrorOverflow) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;



  auto buf = "test := 0xffffffffffffffffffaaaaaaa test";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  try {
    lexer.lex();
  } catch (std::exception &) {
    // Do nothing ...
    return;
  }
}
TEST(LexTest, DigitOutOfRange) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;



  auto buf = "invalid := 0xfffzz";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);

  try {
    lexer.lex();
  } catch(std::exception&) {
    return;
  }

}


TEST(LexTest, HexEscapeOutOfRange) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;



  auto buf = R"(failed_string := "Hello\x2z")";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  try {
    lexer.lex();
  } catch (std::exception &) {
    // Do nothing ...
    return;
  }
}


TEST(LexTest, InvalidStringEscape) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto buf = R"(extern fail_again := "Hello Test\q")";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  try {
    lexer.lex();
  } catch (std::exception &) {
    // Do nothing ...
    return;
  }

}

TEST(LexTest, MissingClosingDelimString) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto buf = R"(export fail_again := "Hello Test\n)";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  try {
    lexer.lex();
  } catch (std::exception &) {
    // Do nothing ...
    return;
  }
}

TEST(LexTest, MissingClosingDelimChar) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;


  auto buf = R"(test := 'a)";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  try {
    lexer.lex();
  } catch (std::exception &) {
    // Do nothing ...
    return;
  }
}


TEST_SUITE_MAIN(LexTest) {
  TEST_CALL(LexTest, NonFMTNumbers);
  TEST_CALL(LexTest, DecimalNumber);
  TEST_CALL(LexTest, HexadecimalNumber);
  TEST_CALL(LexTest, BinaryNumber);
  TEST_CALL(LexTest, OctalNumber);
  TEST_CALL(LexTest, Base36Number);
  TEST_CALL(LexTest, Identifier);
  TEST_CALL(LexTest, Keyword);
  TEST_CALL(LexTest, String);
  TEST_CALL(LexTest, StringEscape);
  TEST_CALL(LexTest, AllTokens);
  TEST_CALL(LexTest, ErrorOverflow);
  TEST_CALL(LexTest, DigitOutOfRange);
  TEST_CALL(LexTest, HexEscapeOutOfRange);
  TEST_CALL(LexTest, InvalidStringEscape);
  TEST_CALL(LexTest, MissingClosingDelimString);
  TEST_CALL(LexTest, MissingClosingDelimChar);
}