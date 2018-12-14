#include "algorithms/node_distributed_connected_components.hpp"

#include <cmath>
#include <vector>

#include <mpi.h>
#include <omp.h>

#include "graph/distributed_hook_tree.hpp"
#include "util/ceil_div.hpp"
#include "util/timer.hpp"

namespace algorithms {

namespace {
void checkMPI(int ret) {
  assert(ret == MPI_SUCCESS);
}
}

graph::DistributedHookTree nodeDistributedConnectedComponents(
    const Label n_vertices, const std::vector<graph::Edge>& all_edges, Label n_threads_per_node,
    double* computation_time, double* total_time) {
  const auto start = util::getTime();

  // get MPI params
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  const std::size_t n_edges = all_edges.size();

  std::vector<graph::Edge> internal_edges;
  std::vector<graph::Edge> boundary_edges;

  using graph::Label;
  const Label vertices_per_rank = util::ceilDiv(n_vertices, comm_size);
  const Label start_index = vertices_per_rank * rank;
  const Label end_index = vertices_per_rank * (rank + 1);
  auto is_internal = [=](Label l) { return l >= start_index && l < end_index; };

  for (const auto& e : all_edges) {
    if (is_internal(e.first) && is_internal(e.second)) {
      internal_edges.push_back(e);
    }
    // TODO: consider ordering.
    else if (is_internal(e.first)) {
      boundary_edges.push_back(e);
    }
  }

  // Start the timer.
  MPI_Barrier(MPI_COMM_WORLD);
  const auto start_computation = util::getTime();

  graph::DistributedHookTree tree(vertices_per_rank, rank, comm_size, n_threads_per_node);

// Create local forest.

#pragma omp parallel for num_threads(n_threads_per_node) schedule(dynamic, 5000)
  for (Label e_id = 0; e_id < internal_edges.size(); ++e_id) {
    const auto& edge = internal_edges[e_id];

    tree.hookToMinSafeLocal(edge.first, edge.second);
  }

  tree.compressLocal();

  tree.sync();

// Again, but with boundary edges.
#pragma omp parallel for num_threads(n_threads_per_node) schedule(dynamic, 100)
  for (Label e_id = 0; e_id < boundary_edges.size(); ++e_id) {
    const auto& edge = boundary_edges[e_id];

    tree.hookToMinSafe(edge.first, edge.second);
  }

  tree.sync();
  tree.compress();

  tree.sync();//MPI_Barrier(MPI_COMM_WORLD);
  const auto end = util::getTime();

  if (computation_time)
    *computation_time = util::getDiff(start_computation, end);
  if (total_time)
    *total_time = util::getDiff(start, end);

  return tree;
}

graph::HookTree gatherTree(const graph::DistributedHookTree& local_tree, const Label n_vertices) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  std::vector<graph::Label> representatives((rank == 0) * (comm_size * local_tree.size()));

  auto MPI_type = parallel::MPITypeMap<Label>::value();
  checkMPI(MPI_Gather(local_tree.data(), local_tree.size(), MPI_type, representatives.data(),
                      local_tree.size(), MPI_type, 0, MPI_COMM_WORLD));

  if (n_vertices != -1)
    representatives.resize(n_vertices);
  return graph::HookTree(std::move(representatives));
}

}  // algorithms
