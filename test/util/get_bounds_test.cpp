#include "util/get_bounds.hpp"

#include <gtest/gtest.h>

TEST(GetBoundsTests, MoreTasksThanOnjects) {
  const int n_tasks = 5;
  const int n_objects = 3;
  const auto external_bounds = std::make_pair(0, n_objects);

  using util::getBounds;
  EXPECT_EQ(std::make_pair(0, 1), getBounds(0, n_tasks, external_bounds));
  EXPECT_EQ(std::make_pair(1, 2), getBounds(1, n_tasks, external_bounds));
  EXPECT_EQ(std::make_pair(2, 3), getBounds(2, n_tasks, external_bounds));

  for (int i = n_objects; i < n_tasks; ++i) {
    const auto bounds = getBounds(i, n_tasks, external_bounds);
    EXPECT_EQ(bounds.first, bounds.second);
  }
}
