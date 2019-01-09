#include "algorithms/parallel_connected_components.hpp"

#include <omp.h>

namespace algorithms {

graph::HookTree parallelConnectedComponents(const unsigned n, std::vector<graph::Edge>& edges,
                                            const int n_threads) {
  const unsigned int m = edges.size();

  if (n_threads > 0) {
    omp_set_num_threads(n_threads);
  }

  graph::HookTree tree(n, true);

  bool failures = true;

  while (failures) {
    failures = false;
#pragma omp parallel for schedule(dynamic, 5000)
    for (unsigned int e_id = 0; e_id < m; ++e_id) {
      auto& edge = edges[e_id];
//      if (!edge.isValid())
//        continue;

      tree.hookToMinSafe(edge.first, edge.second);
    }

#pragma omp parallel for schedule(dynamic, 5000)
    for (unsigned int i = 0; i < n; ++i) {
      tree.compress(i);
    }
  }

  return tree;
}

}  // algorithms
