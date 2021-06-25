#include <gtest/gtest.h>
#include "nextgen/Support/Core.h"
#include "nextgen/Support/Allocator.h"

TEST(Lists, ListTest) {
  using namespace nextgen;
  using namespace nextgen::core;

  auto list = mem::Vec<int>{};
  list.Add(23);
  list.Add(24);
  list.Add(53554);
  list.Add(3554);
  list.Add(3554);
  list.Add(3554);

  ASSERT_EQ(list.At(0), 23);
  ASSERT_EQ(list.At(1), 24);
  ASSERT_EQ(list.At(2), 53554);
  ASSERT_EQ(list.At(3), 3554);
}
