#pragma once

#include "graph/hook_tree.hpp"

namespace graph {

// Returns the size of each connected components.
std::vector<Label> connectedComponentsSize(const HookTree& tree);

} // graph
