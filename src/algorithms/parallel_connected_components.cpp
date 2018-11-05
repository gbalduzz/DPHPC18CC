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
#pragma omp parallel for
    for (unsigned int e_id = 0; e_id < m; ++e_id) {
      auto& edge = edges[e_id];
      if (!edge.isValid())
        continue;

      const auto repr_i = tree.representative(edge.first);
      const auto repr_j = tree.representative(edge.second);

      if (repr_i != repr_j) {
        const bool hooked = tree.hookAtomic(std::max(repr_i, repr_j), std::min(repr_i, repr_j));

        if (!hooked) {
          failures = true;
          continue;
        }
      }

      edge.markInvalid();
    }

#pragma omp parallel for
    for (unsigned int i = 0; i < n; ++i) {
      tree.compress(i);
    }
  }

  return tree;
}

}  // algorithms
