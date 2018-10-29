
#include "algorithms/serial_connected_components.hpp"

namespace algorithms {

graph::mock::HookTree serialConnectedComponents(const unsigned n,
                                                const std::vector<graph::Edge>& edges) {
  const unsigned int m = edges.size();

  graph::mock::HookTree tree(n);

  for (const auto& edge : edges) {
    // Assume ordered edge.
    tree.hook(edge.first, edge.second);
  }

  while (true) {
    bool changes = false;
    for (const auto& edge : edges) {
      const auto i = edge.first;
      const auto j = edge.second;

      if (tree.isStar(i) && tree.representative(i) != tree.representative(j)) {
        tree.hook(i, j);
        changes = true;
      }
    }

    changes &= tree.compress();

    if (!changes)
      break;
  }

  return tree;
}

}  // algorithms
