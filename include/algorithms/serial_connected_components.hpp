#pragma once

#include <vector>

#include "graph/mock_hook_tree.hpp"
#include "graph/edge.hpp"

namespace algorithms {

graph::mock::HookTree serialConnectedComponents(unsigned int n,
                                                std::vector<graph::Edge>& edges);

}  // algorithms
