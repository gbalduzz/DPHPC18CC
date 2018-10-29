#include "algorithms/serial_connected_components.hpp"

#include <gtest/gtest.h>

void performTest(int n, const std::vector<graph::Edge>& edges, const std::vector<int>& expected);

//TEST(SerialConnectedComponentsTest, Simple) {
//  const int n = 5;
//  const std::vector<graph::Edge> edges{std::make_pair(1, 0), std::make_pair(2, 1),
//                                       std::make_pair(2, 0), std::make_pair(4, 3)};
//  const std::vector<int> expected{0, 0, 0, 1, 1};
//
//  performTest(n, edges, expected);
//}

TEST(SerialConnectedComponentsTest, LessSimple) {
  const int n = 12;
  const std::vector<graph::Edge> edges{
      std::make_pair(8, 2),  std::make_pair(5, 1), std::make_pair(3, 0),
      std::make_pair(6, 3),  std::make_pair(6, 0), std::make_pair(5, 4),
      std::make_pair(11, 6), std::make_pair(7, 4), std::make_pair(10, 9)};

  const std::vector<int> expected{0, 1, 2, 0, 1, 1, 0, 1, 2, 3, 3, 0};

  performTest(n, edges, expected);
}

void performTest(int n, const std::vector<graph::Edge>& edges, const std::vector<int>& expected) {
  auto forest = algorithms::serialConnectedComponents(n, edges);

  auto are_connected = [&](int i, int j) {
    return forest.representative(i) == forest.representative(j);
  };
  auto expect_connected = [&](int i, int j) { return expected[i] == expected[j]; };

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      EXPECT_EQ(expect_connected(i, j), are_connected(i, j));
}
