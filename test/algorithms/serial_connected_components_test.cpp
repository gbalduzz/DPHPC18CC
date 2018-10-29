#include "algorithms/serial_connected_components.hpp"

#include <gtest/gtest.h>

TEST(SerialConnectedComponentsTest, All) {
  const int n = 5;
  std::vector<graph::Edge> edges{std::make_pair(1, 0), std::make_pair(2, 1), std::make_pair(2, 0),
                                 std::make_pair(4, 3)};

  auto forest = algorithms::serialConnectedComponents(n, edges);

  auto are_connected = [&](int i, int j) {
    return forest.representative(i) == forest.representative(j);
  };

  for (int i = 0; i <= 2; ++i)
    EXPECT_TRUE(are_connected(0, i));

  EXPECT_FALSE(are_connected(0, 3));

  for (int i = 3; i <= 4; ++i)
    EXPECT_TRUE(are_connected(3, i));
}
