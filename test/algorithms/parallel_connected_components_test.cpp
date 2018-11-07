#include "algorithms/parallel_connected_components.hpp"

#include <fstream>
#include <gtest/gtest.h>

#include "graph/generate_random_graph.hpp"

void performTest(int n, std::vector<graph::Edge>& edges, const std::vector<int>& expected);

using graph::Edge;

TEST(ParallelConnectedComponentsTest, Simple) {
  const int n = 5;
  std::vector<graph::Edge> edges{Edge(1, 0), Edge(2, 1), Edge(2, 0), Edge(4, 3)};
  const std::vector<int> expected{0, 0, 0, 1, 1};

  performTest(n, edges, expected);
}

TEST(ParallelConnectedComponentsTest, LessSimple) {
  const int n = 12;
  std::vector<graph::Edge> edges{Edge(8, 2), Edge(5, 1),  Edge(3, 0), Edge(6, 3), Edge(6, 0),
                                       Edge(5, 4), Edge(11, 6), Edge(7, 4), Edge(10, 9)};

  const std::vector<int> expected{0, 1, 2, 0, 1, 1, 0, 1, 2, 3, 3, 0};

  performTest(n, edges, expected);
}

TEST(ParallelConnectedComponentsTest, Precomputed) {
    const int n = 200;
    const int m = 150;
    auto edges = graph::generateRandomGraph(n,m);

    std::vector<int> expected(n);
    std::ifstream inp("labels.txt");
    for(auto& label : expected)
        inp >> label;
    inp.close();

    performTest(n, edges, expected);
}

void performTest(int n, std::vector<graph::Edge>& edges, const std::vector<int>& expected) {
  auto forest = algorithms::parallelConnectedComponents(n, edges, 4);

  auto are_connected = [&](int i, int j) {
    return forest.representative(i) == forest.representative(j);
  };
  auto expect_connected = [&](int i, int j) { return expected[i] == expected[j]; };

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      EXPECT_EQ(expect_connected(i, j), are_connected(i, j));
}
