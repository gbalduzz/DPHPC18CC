#include "graph/connected_components_analysis.hpp"

namespace graph {

std::vector<Label> connectedComponentsSize(const HookTree& tree) {
  std::vector<Label> counts(tree.size(), 0);

  for (int i = 0; i < counts.size(); ++i)
    ++counts[tree.parent(i)];

  counts.erase(std::remove(counts.begin(), counts.end(), 0), counts.end());

  return counts;
}

}  // graph
