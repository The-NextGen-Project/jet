#include <gtest/gtest.h>
#include "nextgen/support/core.h"
#include "nextgen/support/allocator.h"

TEST(Lists, ListTest) {
  using namespace nextgen;
  using namespace nextgen::core;

  auto list = mem::Vec<int>::New();
  list.Add(23);
  list.Add(24);
  list.Add(53554);
  list.Add(3554);
  list.Add(3554);
  list.Add(3554);

  ASSERT_EQ(list[0], 23);
  ASSERT_EQ(list[1], 24);
  ASSERT_EQ(list[2], 53554);
  ASSERT_EQ(list[3], 3554);
}
