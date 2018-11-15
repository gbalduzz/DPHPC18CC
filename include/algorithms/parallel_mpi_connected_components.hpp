#pragma once

#include <vector>
#include <string>


#include "graph/hook_tree.hpp"
#include "graph/edge.hpp"

namespace algorithms {

    graph::HookTree parallelMpiConnectedComponents(std::string graph_file_name, int n_threads_per_node);

}  // algorithms
