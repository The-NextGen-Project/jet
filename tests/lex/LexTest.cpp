#include <gtest/gtest.h>
#include <nextgen/jet/lex/lexer.h>

TEST(LexTest, NonFMTNumbers) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer {
    arena.begin,
    "1111",
    4
  };
  auto u_token = lexer.NextToken().Unwrap();

  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 1111);
  ASSERT_EQ(u_token.name(), "1111"_intern);
  ASSERT_EQ(u_token.name().getHashCache(), "1111"_intern.getHashCache());
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, HexadecimalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer {
    arena.begin,
    "0x1111",
    6
  };
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 0x1111);
  ASSERT_EQ(u_token.name(), "0x1111"_intern);
  ASSERT_EQ(u_token.name().getHashCache(), "0x1111"_intern.getHashCache());
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, BinaryNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer {
    arena.begin,
    "0b011101110",
    11
  };
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 0b011101110);
  ASSERT_EQ(u_token.name(), "0b011101110"_intern);
  ASSERT_EQ(u_token.name().getHashCache(), "0b011101110"_intern.getHashCache());
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, OctalNumber) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer {
    arena.begin,
    "0112022",
    7
  };
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 0112022);
  ASSERT_EQ(u_token.name(), "0112022"_intern);
  ASSERT_EQ(u_token.name().getHashCache(), "0112022"_intern.getHashCache());
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Base36Number) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer {
    arena.begin,
    "0#helloworld",
    12
  };
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.getValue<decltype(UINTPTR_MAX)>(), 1767707668033969);
  ASSERT_EQ(u_token.name(), "0#helloworld"_intern);
  ASSERT_EQ(u_token.name().getHashCache(), "0#helloworld"_intern.getHashCache());
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Integer);
}

TEST(LexTest, Identifier) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer {
    arena.begin,
    "ident",
    5
  };
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.name(), "ident"_intern);
  ASSERT_EQ(u_token.name().getHashCache(), "ident"_intern.getHashCache());
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::Identifier);
}

TEST(LexTest, Keyword) {
  using namespace nextgen;
  using namespace nextgen::core;

  Arena<2> arena;
  auto lexer = jet::Lexer {
    arena.begin,
    "while extern",
    12
  };
  auto u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.name(), "while"_intern);
  ASSERT_EQ(u_token.name().getHashCache(), "while"_intern.getHashCache());
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::KeywordWhile);

  u_token = lexer.NextToken().Unwrap();
  ASSERT_EQ(u_token.name(), "extern"_intern);
  ASSERT_EQ(u_token.name().getHashCache(), "extern"_intern.getHashCache());
  ASSERT_EQ(u_token.getKind(), jet::TokenKind::KeywordExtern);
}