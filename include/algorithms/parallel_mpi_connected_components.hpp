#pragma once

#include <vector>
#include <string>


#include "graph/hook_tree.hpp"
#include "graph/edge.hpp"

namespace algorithms {

    //Returns the hooked tree and the time after the distribution of the edges.
    std::pair<graph::HookTree, double> parallelMpiConnectedComponents(std::vector<graph::Edge>& all_edges, int n_threads_per_node);

}  // algorithms
