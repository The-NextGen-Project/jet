#include <gtest/gtest.h>
#include "nextgen/core-util.h"
#include "nextgen/allocator.h"

TEST(Lists, ListTest) {
  using namespace nextgen;
  using namespace nextgen::mem;

  int *list = nullptr;
  vec::push(list, 23);
  vec::push(list, 24);
  vec::push(list, 23);
  vec::push(list, 53554);
  vec::push(list, 3554);

  ASSERT_EQ(list[0], 23);
  ASSERT_EQ(list[1], 24);
  ASSERT_EQ(list[2], 23);
  ASSERT_EQ(list[3], 53554);
  ASSERT_EQ(list[4], 3554);
}
