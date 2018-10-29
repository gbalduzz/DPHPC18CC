#pragma once

#include <random>
#include <utility>

namespace graph {

// TODO: agree on vertex label type.
using Edge = std::pair<int, int>;

std::vector<Edge> generateRandomGraph(int n, int m) {
  std::mt19937_64 rng(42);
  std::uniform_int_distribution<int> distro(0, n - 1);

  std::vector<Edge> list(m);
  for (auto& edge : list)
    edge = std::make_pair(distro(rng), distro(rng));

  return list;
}

}  // graph
