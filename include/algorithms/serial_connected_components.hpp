#pragma once

#include <vector>

#include "graph/hook_tree.hpp"
#include "graph/typedefs.hpp"

namespace algorithms {

graph::HookTree serialConnectedComponents(unsigned int n, const std::vector<graph::Edge>& edges);

}  // algorithms
