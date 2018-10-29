
#include "algorithms/serial_connected_components.hpp"

namespace algorithms {

graph::HookTree serialConnectedComponents(const unsigned n, const std::vector<graph::Edge>& edges) {
  const unsigned int m = edges.size();

  graph::HookTree tree(n);

  for (const auto& edge : edges) {
  }

  return tree;
}

}  // algorithms
