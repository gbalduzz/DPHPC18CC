#include "algorithms/parallel_mpi_connected_components.hpp"

#include <fstream>
#include <memory>
#include <gtest/gtest.h>

#include "graph/generate_random_graph.hpp"
#include "parallel/concurrency/mpi_concurrency.hpp"
#include "util/graph_reader.hpp"

void performTest(int n, const std::vector<graph::Edge>& edges, const std::vector<int>& expected);

using graph::Edge;

std::unique_ptr<parallel::MpiConcurrency> concurrency;

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
  auto edges = graph::generateRandomGraph(n, m);

  std::vector<int> expected(n);
  std::ifstream inp("labels.txt");
  for (auto& label : expected)
    inp >> label;
  inp.close();

  performTest(n, edges, expected);
}

TEST(ParallelMpiConnectedComponents, Random) {
  // Load random graph
  const std::string filename = "graph_0_info.txt";
  std::ifstream inp(filename);
  std::vector<int> expected;

  std::string content;
  while (std::getline(inp, content)) {
    expected.push_back(std::stoi(content));
  }
  inp.close();

  util::GraphReader reader;
  auto edges = reader.read_graph_from_adjacency_list("graph_0.adjlist");

  performTest(expected.size(), edges, expected);
}

void performTest(int n, const std::vector<graph::Edge>& edges, const std::vector<int>& expected) {
  std::vector<graph::Edge> edges_copy;

  for (const int n_threads : std::array<int, 2>{1, 4}) {
    if (concurrency->id() == 0) {
      edges_copy = edges;
    }

    auto forest = algorithms::parallelMpiConnectedComponents(edges_copy, n_threads);

    auto are_connected = [&](int i, int j) {
      return forest.representative(i) == forest.representative(j);
    };
    auto expect_connected = [&](int i, int j) { return expected[i] == expected[j]; };

    if (concurrency->id() == 0) {
      for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
          EXPECT_EQ(expect_connected(i, j), are_connected(i, j));
    }
  }
}

int main(int argc, char** argv) {
  concurrency = std::make_unique<parallel::MpiConcurrency>(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  concurrency.release();
  return result;
}
