#pragma once

#include <vector>

#include "graph/hook_tree.hpp"
#include "graph/edge.hpp"

namespace algorithms {

graph::HookTree parallelConnectedComponents(unsigned int n, std::vector<graph::Edge>& edges,
                                            int n_threads, int block_size);

}  // algorithms
