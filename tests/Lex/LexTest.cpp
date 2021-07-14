#include <gtest/gtest.h>
#include <nextgen/jet/Lex/Lexer.h>
#include <nextgen/jet/Diagnostics.h>

TEST(LexTest, NonFMTNumbers) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer (Arena.Begin,"1111","src/test.jet", 4);
  auto Token = Lexer.NextToken();

  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 1111);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, HexadecimalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto lexer = jet::Lexer(Arena.Begin, "0x1111", "src/test.jet", 6);
  auto Token = lexer.NextToken();
  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 0x1111);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, BinaryNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer(Arena.Begin, "0b011101110", "src/test.jet", 11);
  auto Token = Lexer.NextToken();
  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 0b011101110);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, OctalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer(Arena.Begin, "0112022", "src/test.jet", 7);
  auto Token = Lexer.NextToken();
  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 0112022);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Base36Number) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer(Arena.Begin, "0#helloworld", "src/test.jet", 12);
  auto Token = Lexer.NextToken();

  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 1767707668033969);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Identifier) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer(Arena.Begin, "ident", "src/test.jet", 5);
  auto Token = Lexer.NextToken();
  ASSERT_EQ(Token.name().getHashCache(), "ident"_intern.getHashCache());
  ASSERT_EQ(Token.name(), "ident"_intern);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Identifier);
}

TEST(LexTest, Keyword) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer(Arena.Begin, "while extern", "src/test.jet",
                               12);
  auto Token = Lexer.NextToken();
  ASSERT_EQ(Token.name().getHashCache(), "while"_intern.getHashCache());
  ASSERT_EQ(Token.getKind(), jet::TokenKind::KeywordWhile);
  ASSERT_EQ(Token.name(), "while"_intern);


  Token = Lexer.NextToken();
  ASSERT_EQ(Token.name().getHashCache(), "extern"_intern.getHashCache());
  ASSERT_EQ(Token.name(), "extern"_intern);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::KeywordExtern);
}

TEST(LexTest, String) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer(Arena.Begin, R"("Hello" ace "There")",
                               "src/test.jet",19);
  auto Token = Lexer.NextToken();
  ASSERT_EQ(Token.name().getHashCache(), "Hello"_intern.getHashCache());
  ASSERT_EQ(Token.getKind(), jet::TokenKind::String);
  ASSERT_EQ(Token.name(), "Hello"_intern);

  Token = Lexer.NextToken();
  ASSERT_EQ(Token.name().getHashCache(), "ace"_intern.getHashCache());
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Identifier);
  ASSERT_EQ(Token.name(), "ace"_intern);

  Token = Lexer.NextToken();
  ASSERT_EQ(Token.name().getHashCache(), "There"_intern.getHashCache());
  ASSERT_EQ(Token.getKind(), jet::TokenKind::String);
  ASSERT_EQ(Token.name(), "There"_intern);
}

TEST(LexTest, StringEscape) { // TODO: Add Unicode Escape Later

  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = R"(var every_escape = "I have\x56 every \t thing\n that could \b\v\a in \r")";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Token = Lexer.NextToken();


  ASSERT_EQ(Token.name().getHashCache(), "I have\x56 every \t thing\n that could \b\v\a in \r"_intern
                                                        .getHashCache());
  ASSERT_EQ(Token.name(), "I have\x56 every \t thing\n that could \b\v\a in \r"_intern);
}

TEST(LexTest, AllTokens) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = "+ += ++ - -= -- * *=  ** **= / /= > >= >> >>= < <= << <<= % "
             "() {} "
             "[] ~ ^ "
             "| |= & &= = == : ; @ ! ? . .. ..."; // 43 Tokens
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", Len);


  auto Tokens = Lexer.Lex();
 ASSERT_EQ(Tokens.size(), 44); /* Test for EOF Too */
/*  ASSERT_EQ(Tokens[0].getKind(), jet::TokenKind::Plus);
  ASSERT_EQ(Tokens[1].getKind(), jet::TokenKind::PlusEquals);
  ASSERT_EQ(Tokens[2].getKind(), jet::TokenKind::PlusPlus);
  ASSERT_EQ(Tokens[3].getKind(), jet::TokenKind::Minus);
  ASSERT_EQ(Tokens[4].getKind(), jet::TokenKind::MinusEquals);
  ASSERT_EQ(Tokens[5].getKind(), jet::TokenKind::MinusMinus);
  ASSERT_EQ(Tokens[6].getKind(), jet::TokenKind::Star);
  ASSERT_EQ(Tokens[7].getKind(), jet::TokenKind::MulEquals);
  ASSERT_EQ(Tokens[8].getKind(), jet::TokenKind::Pow);
  ASSERT_EQ(Tokens[9].getKind(), jet::TokenKind::Slash);*/

}

TEST(LexTest, ErrorOverflow) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = "var test = 0xffffffffffffffffffaaaaaaa test";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken();

  jet::Diagnostic Diagnostic = jet::Diagnostic(Arena.Begin, Buf, Len,"src/test"
                                                                 ".jet");
}

TEST(LexTest, DigitOutOfRange) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = "var invalid = 0xfffzz";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken();
}

TEST(LexTest, HexEscapeOutOfRange) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = R"(var failed_string = "Hello\x2z")";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken();
}

TEST(LexTest, InvalidStringEscape) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Buf = R"(extern fail_again = "Hello Test\q")";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken();

}

TEST(LexTest, MissingClosingDelimString) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Buf = R"(export fail_again = "Hello Test\n)";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken();
}

TEST(LexTest, MissingClosingDelimChar) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Buf = R"(var test = 'a)";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken();
}