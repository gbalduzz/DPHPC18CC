#pragma once

#include <vector>
#include <string>


#include "graph/hook_tree.hpp"
#include "graph/edge.hpp"

namespace algorithms {

    graph::HookTree parallelMpiConnectedComponents(std::vector<graph::Edge>& all_edges, int n_threads_per_node);

}  // algorithms
