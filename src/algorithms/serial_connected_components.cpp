
#include "algorithms/serial_connected_components.hpp"

namespace algorithms {

graph::mock::HookTree serialConnectedComponents(const unsigned n,
                                                const std::vector<graph::Edge>& edges) {
  const unsigned int m = edges.size();

  graph::mock::HookTree tree(n);

  for (const auto& edge : edges) {
    // Assume ordered edge.
    if (tree.representative(edge.first) == edge.first)
      tree.hook(edge.first, edge.second);
  }

  while (true) {
    bool changes = false;
    for (const auto& edge : edges) {
      const auto i = edge.first;
      const auto j = edge.second;

      // NOTE: the call to isStaar does not seem to be a prerequisite for correctness.
      if (tree.isStar(i) && tree.representative(i) > tree.representative(j)) {
        changes = true;
        tree.hook(i, j);
      }
      else if (tree.isStar(j) && tree.representative(j) > tree.representative(i)) {
        changes = true;
        tree.hook(j, i);
      }
    }

    tree.compress();

    if (!changes)
      break;
  }

  return tree;
}

}  // algorithms
