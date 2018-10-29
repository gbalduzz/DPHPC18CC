#pragma once

#include <vector>

#include "graph/mock_hook_tree.hpp"
#include "graph/typedefs.hpp"

namespace algorithms {

graph::mock::HookTree serialConnectedComponents(unsigned int n,
                                                const std::vector<graph::Edge>& edges);

}  // algorithms
