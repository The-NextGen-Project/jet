#include <gtest/gtest.h>
#include <nextgen/jet/Lex/Lexer.h>
#include <nextgen/jet/Diagnostics.h>


TEST(LexTest, NonFMTNumbers) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer::New (Arena.Begin,"1111","src/test.jet", 4);
  auto Token = Lexer.NextToken().Unwrap();

  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 1111);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, HexadecimalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto lexer = jet::Lexer::New(Arena.Begin, "0x1111", "src/test.jet", 6);
  auto Token = lexer.NextToken().Unwrap();
  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 0x1111);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, BinaryNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer::New(Arena.Begin, "0b011101110", "src/test.jet", 11);
  auto Token = Lexer.NextToken().Unwrap();
  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 0b011101110);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, OctalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer::New(Arena.Begin, "0112022", "src/test.jet", 7);
  auto Token = Lexer.NextToken().Unwrap();
  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 0112022);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Base36Number) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer::New(Arena.Begin, "0#helloworld", "src/test.jet", 12);
  auto Token = Lexer.NextToken().Unwrap();

  ASSERT_EQ(Token.getValue<decltype(UINTPTR_MAX)>(), 1767707668033969);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Identifier) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer::New(Arena.Begin, "ident", "src/test.jet", 5);
  auto Token = Lexer.NextToken().Unwrap();
  ASSERT_EQ(Token.Name().getHashCache(), "ident"_intern.getHashCache());
  ASSERT_EQ(Token.Name(), "ident"_intern);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Identifier);
}

TEST(LexTest, Keyword) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer::New(Arena.Begin, "while extern", "src/test.jet",
                               12);
  auto Token = Lexer.NextToken().Unwrap();
  ASSERT_EQ(Token.Name().getHashCache(), "while"_intern.getHashCache());
  ASSERT_EQ(Token.getKind(), jet::TokenKind::KeywordWhile);
  ASSERT_EQ(Token.Name(), "while"_intern);


  Token = Lexer.NextToken().Unwrap();
  ASSERT_EQ(Token.Name().getHashCache(), "extern"_intern.getHashCache());
  ASSERT_EQ(Token.Name(), "extern"_intern);
  ASSERT_EQ(Token.getKind(), jet::TokenKind::KeywordExtern);
}

TEST(LexTest, String) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Lexer = jet::Lexer::New(Arena.Begin, R"("Hello" ace "There")",
                               "src/test.jet",19);
  auto Token = Lexer.NextToken().Unwrap();
  ASSERT_EQ(Token.Name().getHashCache(), "Hello"_intern.getHashCache());
  ASSERT_EQ(Token.getKind(), jet::TokenKind::String);
  ASSERT_EQ(Token.Name(), "Hello"_intern);

  Token = Lexer.NextToken().Unwrap();
  ASSERT_EQ(Token.Name().getHashCache(), "ace"_intern.getHashCache());
  ASSERT_EQ(Token.getKind(), jet::TokenKind::Identifier);
  ASSERT_EQ(Token.Name(), "ace"_intern);

  Token = Lexer.NextToken().Unwrap();
  ASSERT_EQ(Token.Name().getHashCache(), "There"_intern.getHashCache());
  ASSERT_EQ(Token.getKind(), jet::TokenKind::String);
  ASSERT_EQ(Token.Name(), "There"_intern);
}

TEST(LexTest, StringEscape) { // TODO: Add Unicode Escape Later

  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = R"(var every_escape = "I have\x56 every \t thing\n that could \b\v\a in \r")";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer::New(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Token = Lexer.NextToken().Unwrap();


  ASSERT_EQ(Token.Name().getHashCache(), "I have\x56 every \t thing\n that could \b\v\a in \r"_intern
                                                        .getHashCache());
  ASSERT_EQ(Token.Name(), "I have\x56 every \t thing\n that could \b\v\a in \r"_intern);
}

TEST(LexTest, AllTokens) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = "+ += - -= * *= **= / /= > >= >> >>= < <= << <<= % () {} [] ~ ^ "
             "| & = == : ; @ ! ? . .. ...";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer::New(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Token = Lexer.NextToken().Unwrap();
}

TEST(LexTest, ErrorOverflow) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = "var test = 0xffffffffffffffffffaaaaaaa test";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer::New(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken().Error().Unwrap();

  jet::Diagnostic Diagnostic = jet::Diagnostic(Arena.Begin, Buf, Len,"src/test"
                                                                 ".jet");

  ASSERT_EQ(Error.Error, jet::LexErrorType::IntegerOverflow);
  Diagnostic.Build(Error);
}

TEST(LexTest, DigitOutOfRange) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = "var invalid = 0xfffzz";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer::New(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken().Error().Unwrap();

  jet::Diagnostic Diagnostic = jet::Diagnostic(Arena.Begin, Buf, Len,"src/test.jet");

  ASSERT_EQ(Error.Error, jet::LexErrorType::DigitOutOfRange);
  ASSERT_EQ(Error.Metadata[0], 16);
  ASSERT_EQ(Error.Metadata[1], 36);
  Diagnostic.Build(Error);
}

TEST(LexTest, HexEscapeOutOfRange) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;

  auto Buf = R"(var failed_string = "Hello\x2z")";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer::New(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken().Error().Unwrap();

  jet::Diagnostic Diagnostic = jet::Diagnostic(Arena.Begin, Buf, Len,"src/test.jet");

  ASSERT_EQ(Error.Error, jet::LexErrorType::HexEscapeOutOfRange);
  ASSERT_EQ(Error.Metadata[0], 36);
  Diagnostic.Build(Error);
}

TEST(LexTest, InvalidStringEscape) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Buf = R"(extern fail_again = "Hello Test\q")";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer::New(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken().Error().Unwrap();

  jet::Diagnostic Diagnostic = jet::Diagnostic(Arena.Begin, Buf, Len,"src/test.jet");

  ASSERT_EQ(Error.Error, jet::LexErrorType::InvalidStringEscape);
  Diagnostic.Build(Error);
}

TEST(LexTest, MissingClosingDelimString) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Buf = R"(export fail_again = "Hello Test\n)";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer::New(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken().Error().Unwrap();

  jet::Diagnostic Diagnostic = jet::Diagnostic(Arena.Begin, Buf, Len,"src/test.jet");

  ASSERT_EQ(Error.Error, jet::LexErrorType::MissingClosingDelim);
  Diagnostic.Build(Error);
}

TEST(LexTest, MissingClosingDelimChar) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> Arena;
  auto Buf = R"(var test = 'a)";
  auto Len = strlen(Buf);
  auto Lexer = jet::Lexer::New(Arena.Begin, Buf, "src/test.jet", Len);

  Lexer.NextToken();
  Lexer.NextToken();
  Lexer.NextToken();
  auto Error = Lexer.NextToken().Error().Unwrap();

  jet::Diagnostic Diagnostic = jet::Diagnostic(Arena.Begin, Buf, Len,"src/test.jet");

  ASSERT_EQ(Error.Error, jet::LexErrorType::MissingClosingDelim);
  Diagnostic.Build(Error);
}