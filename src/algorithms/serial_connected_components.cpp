
#include "algorithms/serial_connected_components.hpp"

namespace algorithms {

graph::HookTree serialConnectedComponents(const unsigned n, std::vector<graph::Edge>& edges) {
  const unsigned int m = edges.size();

  graph::HookTree tree(n);

  for (auto& edge : edges) {
    const auto repr_i = tree.representative(edge.first);
    const auto repr_j = tree.representative(edge.second);

    if (repr_i != repr_j) {
      tree.hook(std::max(repr_i, repr_j), std::min(repr_i, repr_j));
    }
  }

  tree.compress();

  return tree;
}

}  // algorithms
