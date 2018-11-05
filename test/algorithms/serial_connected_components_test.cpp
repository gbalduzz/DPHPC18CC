#include "algorithms/serial_connected_components.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <cstring>
#include "graph/generate_random_graph.hpp"
#include "util/graph_reader.hpp"
#include <iostream>
void performTest( std::vector<graph::Edge>& edges, const std::vector<graph::Label>& expected);
std::vector<graph::Label> readExpected(std::string filename); 
using graph::Edge;


TEST(SerialConnectedComponentsTest, Simple) {
  const int n = 5;
  std::vector<graph::Edge> edges{Edge(1, 0), Edge(2, 1), Edge(2, 0), Edge(4, 3)};
  const std::vector<graph::Label> expected{0, 0, 0, 1, 1};

  performTest(edges, expected);
}

TEST(SerialConnectedComponentsTest, LessSimple) {
  const int n = 12;
  std::vector<graph::Edge> edges{Edge(8, 2), Edge(5, 1),  Edge(3, 0), Edge(6, 3), Edge(6, 0),
                                       Edge(5, 4), Edge(11, 6), Edge(7, 4), Edge(10, 9)};

  const std::vector<graph::Label> expected{0, 1, 2, 0, 1, 1, 0, 1, 2, 3, 3, 0};

  performTest(edges, expected);
}

TEST(SerialConnectedComponentsTest, Precomputed) {
    const int n = 200;
    const int m = 150;
    auto edges = graph::generateRandomGraph(n,m);
    auto expected = readExpected("labels.txt"); 
    performTest(edges, expected);
}

TEST(SerialConnectedComponentsTest, Random) {
    //Load random graph
    util::GraphReader g; 
    auto edges = g.read_graph_from_adjacency_list("graph_0.adjlist"); 
    auto expected = readExpected("graph_0_info.txt"); 
    performTest(edges, expected); 
}

void performTest(std::vector<graph::Edge>& edges, const std::vector<graph::Label>& expected) {
  int n = expected.size(); 
  auto forest = algorithms::serialConnectedComponents(n, edges);

  auto are_connected = [&](int i, int j) {
    return forest.representative(i) == forest.representative(j);
  };
  auto expect_connected = [&](int i, int j) { return expected[i] == expected[j]; };

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) {
      ASSERT_EQ(expect_connected(i, j), are_connected(i, j));
     

    }
     
}


std::vector<graph::Label> readExpected(std::string filename) {

    std::ifstream inp(filename);
    std::vector<graph::Label> expected;

    std::string content; 
    while (std::getline(inp, content)) {
        expected.push_back(std::stoi(content)); 
    }
    inp.close();
    return expected; 
}
