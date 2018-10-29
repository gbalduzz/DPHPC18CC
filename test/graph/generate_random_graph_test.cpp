#include "graph/generate_random_graph.hpp"

#include <gtest/gtest.h>

TEST(GenerateRandomGraphTest, All) {
  const int n = 50;
  const int m = 500;
  auto edges = graph::generateRandomGraph(n, m);

  EXPECT_EQ(500, edges.size());
  for (int i = 0; i < m; ++i) {
    EXPECT_GT(n, std::max(edges[i].first, edges[i].second));
    for (int j = 0; i < i; ++j)
      EXPECT_FALSE(edges[i].first == edges[j].first && edges[i].second == edges[j].second);
  }
}
