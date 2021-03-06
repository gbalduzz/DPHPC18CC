#pragma once

#include <vector>

#include "graph/hook_tree.hpp"
#include "graph/edge.hpp"

namespace algorithms {

graph::HookTree serialConnectedComponents(unsigned int n, const std::vector<graph::Edge>& edges,
                                          int /*threads*/ = 0);

}  // algorithms
