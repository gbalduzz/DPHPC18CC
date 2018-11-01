#pragma once

#include <vector>

#include "graph/hook_tree.hpp"
#include "graph/edge.hpp"

namespace algorithms {

graph::HookTree serialConnectedComponents(unsigned int n,
                                                std::vector<graph::Edge>& edges);

}  // algorithms
