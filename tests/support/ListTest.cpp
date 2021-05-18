#include <gtest/gtest.h>
#include "nextgen/support/core.h"
#include "nextgen/support/allocator.h"

TEST(Lists, ListTest) {
  using namespace nextgen;
  using namespace nextgen::core;

  auto list = mem::list<int> { };
  list.add(23);
  list.add(24);
  list.add(53554);
  list.add(3554);
  list.add(3554);
  list.add(3554);

  ASSERT_EQ(list[0], 23);
  ASSERT_EQ(list[1], 24);
  ASSERT_EQ(list[2], 53554);
  ASSERT_EQ(list[3], 3554);
}
