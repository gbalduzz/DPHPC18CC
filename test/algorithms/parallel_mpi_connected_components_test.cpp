#include "algorithms/parallel_mpi_connected_components.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include "util/graph_reader.hpp"
#include "graph/generate_random_graph.hpp"
#include <string>
#include <mpi.h>
void checkForEquality(graph::HookTree computed, std::vector<graph::Label>& expected, int n);
std::vector<graph::Label> readExpected(std::string filename); 
using graph::Edge;



TEST(ParallelMpiConnectedComponents, Random) {
    //Load random graph
    MPI_Init(nullptr, nullptr); 
    auto result = algorithms::parallelMpiConnectedComponents("graph_0.adjlist",4); 
    auto expected = readExpected("graph_0_info.txt"); 
    checkForEquality(result, expected, expected.size()); 
    MPI_Finalize(); 
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

void checkForEquality(graph::HookTree computed, std::vector<graph::Label>& expected, int n) {
  auto are_connected = [&](int i, int j) {
    return computed.representative(i) == computed.representative(j);
  };
  auto expect_connected = [&](int i, int j) { return expected[i] == expected[j]; };

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      EXPECT_EQ(expect_connected(i, j), are_connected(i, j));
}
