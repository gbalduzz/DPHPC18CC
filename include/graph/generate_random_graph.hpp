#pragma once

#include <algorithm>
#include <random>
#include <utility>

#include "graph/typedefs.hpp"

namespace graph {

std::vector<Edge> generateRandomGraph(int n, int m) {
  constexpr bool ordered = true;

  std::mt19937_64 rng(42);
  std::uniform_int_distribution<int> distro(0, n - 1);

  std::vector<Edge> list;
  list.reserve(m);

  while (list.size() < m) {
    while (list.size() < m) {
      int v1 = distro(rng);
      int v2 = distro(rng);
      if (v1 == v2)
        continue;

      auto edge = (v1 < v2) && ordered ? std::make_pair(v2, v1) : std::make_pair(v1, v2);
      list.push_back(edge);
    }

    std::sort(list.begin(), list.end(), [](Edge e1, Edge e2) {
      return e1.first != e2.first ? e1.first < e2.first : e1.second < e2.second;
    });
    auto end = std::unique(list.begin(), list.end());
    list.resize(end - list.begin());
  }

  return list;
}

}  // graph
