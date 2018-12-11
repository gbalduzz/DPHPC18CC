#pragma once

#include <vector>
#include <string>

#include "graph/distributed_hook_tree.hpp"
#include "graph/hook_tree.hpp"
#include "graph/edge.hpp"

namespace algorithms {

using graph::Label;

// Returns a distributed compressed hooked tree and the time after the distribution of the edges.
graph::DistributedHookTree nodeDistributedConnectedComponents(
    Label n_vertices, const std::vector<graph::Edge>& all_edges, Label n_threads_per_node,
    double* computation_time = nullptr, double* total_time = nullptr);

// Rank 0 returns the gathered tree
graph::HookTree gatherTree(const graph::DistributedHookTree& local_tree, Label n_vertices = -1);

}  // algorithms
