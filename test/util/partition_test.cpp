#include "util/partition.hpp"

#include "gtest/gtest.h"

TEST(PartitionTest, All) {
  using graph::Label;
  auto check_eq = [](Label x, Label y, std::array<Label, 2> a) { return a[0] == x && a[1] == y; };
  using util::partition;

  EXPECT_TRUE(check_eq(2, 2, partition(25, 25, 4)));
  EXPECT_TRUE(check_eq(2, 2, partition(26, 25, 4)));

  EXPECT_TRUE(check_eq(4, 2, partition(100, 50, 8)));
  EXPECT_TRUE(check_eq(4, 2, partition(121, 47, 8)));

  EXPECT_TRUE(check_eq(1, 1, partition(17, 13, 1)));

  EXPECT_TRUE(check_eq(5, 1, partition(500, 100, 5)));

  EXPECT_TRUE(check_eq(5, 2, partition(999, 400, 10)));

  EXPECT_TRUE(check_eq(10, 1, partition(1000, 100, 10)));

  EXPECT_TRUE(check_eq(6, 4, partition(100, 100, 24)) || check_eq(4, 6, partition(100, 100, 24)));
}
