#include <gtest/gtest.h>
#include "nextgen/Support/Core.h"
#include "nextgen/Support/Allocator.h"

TEST(Lists, ListTest) {
  using namespace nextgen;
  using namespace nextgen::core;

  auto list = mem::Vec<int>{};
  list.push(23);
  list.push(24);
  list.push(53554);
  list.push(3554);
  list.push(3554);
  list.push(3554);

  ASSERT_EQ(list[0], 23);
  ASSERT_EQ(list[1], 24);
  ASSERT_EQ(list[2], 53554);
  ASSERT_EQ(list[3], 3554);
}
