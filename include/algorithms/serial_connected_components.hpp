#pragma once

#include <vector>

#include "graph/hook_tree.hpp"
#include "graph/typedefs.hpp"

namespace algorithms {

graph::HookTree serialConnectedComponents(const int n, const std::vector<graph::Edge>& edges);

}  // algorithms
