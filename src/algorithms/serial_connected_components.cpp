
#include "algorithms/serial_connected_components.hpp"

namespace algorithms {

graph::HookTree serialConnectedComponents(const unsigned n, const std::vector<graph::Edge>& edges,
                                          int /*n_threads*/) {
  const unsigned int m = edges.size();

  graph::HookTree tree(n);

  for (auto& edge : edges) {
    if(!edge.isValid())
      continue;

    const auto repr_i = tree.representative(edge.first);
    const auto repr_j = tree.representative(edge.second);

    if (repr_i > repr_j) {
      tree.hookAndUpdate(repr_i, repr_j, edge.first, edge.second);
    }
    else if (repr_i < repr_j) {
      tree.hookAndUpdate(repr_j, repr_i, edge.second, edge.first);
    }
  }

  tree.compress();

  return tree;
}

}  // algorithms
