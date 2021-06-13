#include <gtest/gtest.h>
#include <nextgen/jet/Lex/Lexer.h>

struct ab {
  ab *a;
  ab(ab *a) : a(a) {}

  int size[232];
};

struct ArenaSegment2  {

  ArenaSegment2() = default;

  static auto New() -> ArenaSegment2 {
    return {};
  }

  static auto New(ArenaSegment2 *seg) -> ArenaSegment2 {
    return ArenaSegment2(0, seg);
  }



private:

  ArenaSegment2(size_t Offset, ArenaSegment2 *Next) : Offset(Offset),
                                                    NextSegment(Next) {}

  size_t Offset  = 0;
  ArenaSegment2 *NextSegment = nullptr;
  char    block[(262144)]{};
};

TEST(LexTest, NonFMTNumbers) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer::New (arena.begin,
                                "1111",
                                4);
  auto u_token = lexer.NextToken().Unwrap();

  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 1111);
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, HexadecimalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer::New(arena.begin, "0x1111", 6);
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 0x1111);
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, BinaryNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer::New(arena.begin, "0b011101110", 11);
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 0b011101110);
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, OctalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer::New(arena.begin, "0112022", 7);
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 0112022);
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Base36Number) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer::New(arena.begin, "0#helloworld", 12);
  auto u_token = lexer.NextToken().Unwrap();

  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 1767707668033969);
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Identifier) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer::New(arena.begin, "ident", 5);
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.Name().getHashCache(), "ident"_intern.getHashCache());
  ASSERT_EQ(u_token.Name(), "ident"_intern);
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Identifier);
}

TEST(LexTest, Keyword) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer::New(arena.begin, "while extern", 12);
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.Name().getHashCache(), "while"_intern.getHashCache());
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::KeywordWhile);
  ASSERT_EQ(u_token.Name(), "while"_intern);


  u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.Name().getHashCache(), "extern"_intern.getHashCache());
  ASSERT_EQ(u_token.Name(), "extern"_intern);
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::KeywordExtern);
}