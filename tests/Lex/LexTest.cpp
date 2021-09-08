#include <gtest/gtest.h>
#include <nextgen/jet/jet-lexer.h>
#include <nextgen/jet/jet-diagnostics.h>

TEST(LexTest, NonFMTNumbers) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto lexer = jet::Lexer<TokenMode> ( "1211", "src/test.jet", 4);
  auto token = lexer.lex()[0];

  ASSERT_EQ(token.getValue<decltype(UINTPTR_MAX)>(), 1211);
  ASSERT_EQ(token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, DecimalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto lexer = jet::Lexer<TokenMode> ( "3.1234", "src/test.jet", 6);
  auto token = lexer.lex()[0];

  ASSERT_EQ(token.getValue<double>(), 3.1234);
  ASSERT_EQ(token.getKind(), jet::TokenKind::Decimal);
}


TEST(LexTest, HexadecimalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto lexer = jet::Lexer<TokenMode>( "0x1111", "src/test.jet", 6);
  auto token = lexer.lex()[0];
  ASSERT_EQ(token.getValue<decltype(UINTPTR_MAX)>(), 0x1111);
  ASSERT_EQ(token.getKind(), jet::TokenKind::Integer);
}


TEST(LexTest, BinaryNumber) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto lexer = jet::Lexer<TokenMode>( "0b011101110", "src/test.jet", 11);
  auto token = lexer.lex()[0];
  ASSERT_EQ(token.getValue<decltype(UINTPTR_MAX)>(), 0b011101110);
  ASSERT_EQ(token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, OctalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto lexer = jet::Lexer<TokenMode>( "0112022", "src/test.jet", 7);
  auto token = lexer.lex()[0];
  ASSERT_EQ(token.getValue<decltype(UINTPTR_MAX)>(), 0112022);
  ASSERT_EQ(token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Base36Number) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto lexer = jet::Lexer<TokenMode>( "0#helloworld", "src/test.jet", 12);
  auto token = lexer.lex()[0];

  ASSERT_EQ(token.getValue<decltype(UINTPTR_MAX)>(), 1767707668033969);
  ASSERT_EQ(token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Identifier) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto lexer = jet::Lexer<TokenMode>( "ident", "src/test.jet", 5);
  auto token = lexer.lex()[0];
//  ASSERT_EQ(token.name().getHashCache(), "ident"_intern.getHashCache());
//  ASSERT_EQ(token.name(), "ident"_intern);
  ASSERT_TRUE(::strncmp(token.name().begin(), "ident", 5) == 0);
  ASSERT_EQ(token.getKind(), jet::TokenKind::Identifier);
}

TEST(LexTest, Keyword) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto lexer = jet::Lexer<TokenMode>( "while extern", "src/test.jet",
                          12);

  auto tokens = lexer.lex();
  auto token = tokens[0];
//  ASSERT_EQ(token.name().getHashCache(), "while"_intern.getHashCache());

  ASSERT_TRUE(::strncmp(token.name().begin(), "while", 5) == 0);
  ASSERT_EQ(token.getKind(), jet::TokenKind::KeywordWhile);
//  ASSERT_EQ(token.name(), "while"_intern);


  token = tokens[1];
/*  ASSERT_EQ(token.name().getHashCache(), "extern"_intern.getHashCache());
  ASSERT_EQ(token.name(), "extern"_intern);*/
  ASSERT_TRUE(::strncmp(token.name().begin(), "extern", 6) == 0);
  ASSERT_EQ(token.getKind(), jet::TokenKind::KeywordExtern);
}

TEST(LexTest, String) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto lexer = jet::Lexer<TokenMode>( R"("Hello" ace "There")",
                          "src/test.jet", 19);

  auto tokens = lexer.lex();
  auto token = tokens[0];
//  ASSERT_EQ(token.name().getHashCache(), "Hello"_intern.getHashCache());

  ASSERT_TRUE(::strncmp(token.name().begin(), "Hello", token.len()) == 0);
  ASSERT_EQ(token.getKind(), jet::TokenKind::String);
//  ASSERT_EQ(token.name(), "Hello"_intern);

  token = tokens[1];
//  ASSERT_EQ(token.name().getHashCache(), "ace"_intern.getHashCache());
ASSERT_TRUE(::strncmp(token.name().begin(), "ace", token.len()) == 0);
  ASSERT_EQ(token.getKind(), jet::TokenKind::Identifier);
//  ASSERT_EQ(token.name(), "ace"_intern);

  token = tokens[2];
//  ASSERT_EQ(token.name().getHashCache(), "There"_intern.getHashCache());
ASSERT_TRUE(::strncmp(token.name().begin(), "There", token.len()) == 0);
  ASSERT_EQ(token.getKind(), jet::TokenKind::String);
//  ASSERT_EQ(token.name(), "There"_intern);
}


TEST(LexTest, StringEscape) { // TODO: Add Unicode Escape Later

  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  

  auto buf = R"(var every_escape = "I have\x56 every \t thing\n that could \b\v\a in \r")";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  auto tokens = lexer.lex();
  auto token = tokens[3];
//  ASSERT_EQ(token.name().getHashCache(), "I have\x56 every \t thing\n that could \b\v\a in \r"_intern
//                                                        .getHashCache());

  auto compare = "I have\x56 every \t thing\n that could \b\v\a in \r";
  ASSERT_TRUE(::strncmp(token.name().begin(), compare, token.len()) == 0);
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

  Console::Log((char)buf[108]);

  auto tokens = lexer.lex();
 ASSERT_EQ(vec::size(tokens), 46);
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

  

  auto buf = "var test = 0xffffffffffffffffffaaaaaaa test";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  lexer.lex();

  jet::Diagnostic Diagnostic = jet::Diagnostic( buf, len, "src/test"
                                                                 ".jet");
}
TEST(LexTest, DigitOutOfRange) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  

  auto buf = "var invalid = 0xfffzz";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  lexer.lex();
}


TEST(LexTest, HexEscapeOutOfRange) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  

  auto buf = R"(var failed_string = "Hello\x2z")";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  lexer.lex();
}


TEST(LexTest, InvalidStringEscape) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto buf = R"(extern fail_again = "Hello Test\q")";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  lexer.lex();

}

TEST(LexTest, MissingClosingDelimString) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto buf = R"(export fail_again = "Hello Test\n)";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  lexer.lex();
}

TEST(LexTest, MissingClosingDelimChar) {
  using namespace nextgen;
  using namespace nextgen::core;
  using namespace nextgen::jet;

  
  auto buf = R"(var test = 'a)";
  auto len = strlen(buf);
  auto lexer = jet::Lexer<TokenMode>( buf, "src/test.jet", len);
  lexer.lex();
}
