#include "algorithms/node_distributed_connected_components.hpp"

#include <cmath>
#include <vector>

#include <mpi.h>
#include <omp.h>

#include "algorithms/parallel_connected_components.hpp"
#include "algorithms/serial_connected_components.hpp"
#include "graph/distributed_hook_tree.hpp"
#include "util/timer.hpp"

namespace algorithms {

inline int ceilDiv(int a, int b) {
  return (a + b - 1) / b;
}

void checkMPI(int ret) {
  assert(ret == MPI_SUCCESS);
}

graph::DistributedHookTree nodeDistributedConnectedComponents(std::vector<graph::Edge>& all_edges,
                                                              int n_threads_per_node,
                                                              double* computation_time,
                                                              double* total_time) {
  const auto start = util::getTime();

  // get MPI params
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  int n_edges = -1;
  unsigned int n_vertices = 0;

  n_edges = all_edges.size();
  for (const auto& e : all_edges) {
    n_vertices = std::max(n_vertices, std::max(e.first, e.second));
  }
  ++n_vertices;

  std::vector<graph::Edge> internal_edges;
  std::vector<graph::Edge> boundary_edges;

  using graph::Label;
  const Label start_index = (n_vertices / comm_size) * rank;
  const Label end_index = (n_vertices / comm_size) * (rank + 1);
  auto is_internal = [=](Label l) { return l >= start_index && l < end_index; };

  for (const auto& e : all_edges) {
    if (is_internal(e.first) && is_internal(e.second)) {
        internal_edges.push_back(e);
    }
    else if (is_internal(e.first) || is_internal(e.second)) {
        auto min = std::min(e.first, e.second);
        auto max = std::max(e.first, e.second);
        if(is_internal(max))
            boundary_edges.push_back(graph::Edge(min, max));
    }
  }

  // Start the timer.
  MPI_Barrier(MPI_COMM_WORLD);
  const auto start_computation = util::getTime();

  graph::DistributedHookTree tree(n_vertices, rank, comm_size, n_threads_per_node);

  // Create local forest.
  bool failures = true;
  while (failures) {
    failures = false;
#pragma omp parallel for num_threads(n_threads_per_node) schedule(dynamic, 5000)
    for (unsigned int e_id = 0; e_id < internal_edges.size(); ++e_id) {
      auto& edge = internal_edges[e_id];
      if (!edge.isValid())
        continue;

      const auto repr_i = tree.representativeLocal(edge.first);
      const auto repr_j = tree.representativeLocal(edge.second);

      if (repr_i != repr_j) {
        bool hooked;
        if (repr_i > repr_j)
          hooked = tree.hookAtomicLocal(repr_i, repr_j);
        else
          hooked = tree.hookAtomicLocal(repr_j, repr_i);

        if (hooked) {
          edge.markInvalid();
        }
        else {
          failures = true;
          continue;
        }
      }
    }
  }
  tree.compressLocal();

  // TODO: take care of external edges.

  const auto end = util::getTime();

  if (computation_time)
    *computation_time = util::getDiff(start_computation, end);
  if (total_time)
    *total_time = util::getDiff(start, end);

  return tree;
}

graph::HookTree gatherTree(const graph::DistributedHookTree& local_tree) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  std::vector<graph::Label> representatives((rank == 0) * (comm_size * local_tree.size()));

  MPI_Gather(local_tree.data(), local_tree.size(), MPI_UNSIGNED, representatives.data(),
             local_tree.size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD);

  return graph::HookTree(std::move(representatives));
}

}  // algorithms
