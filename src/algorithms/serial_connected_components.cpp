
#include "algorithms/serial_connected_components.hpp"

namespace algorithms {

graph::HookTree serialConnectedComponents(const unsigned n, std::vector<graph::Edge>& edges) {
  const unsigned int m = edges.size();

  graph::HookTree tree(n);

  while (true) {
    bool changes = false;
    for (auto& edge : edges) {
      if (!edge.isValid())
        continue;
      const auto repr_i = tree.representative(edge.first);
      const auto repr_j = tree.representative(edge.second);

      if (repr_i != repr_j) {
        changes = true;
        const bool hooked = tree.hook(std::max(repr_i, repr_j), std::min(repr_i, repr_j));

        if (hooked) {
          edge.markInvalid();
        }
      }
    }

    tree.compress();

    if (!changes)
      break;
  }

  return tree;
}

}  // algorithms
