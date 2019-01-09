#include "algorithms/parallel_mpi_connected_components.hpp"

#include <cmath>
#include <vector>

#include <mpi.h>
#include <omp.h>
#include <parallel/concurrency/mpi_type_map.hpp>

#include "algorithms/parallel_connected_components.hpp"
#include "algorithms/serial_connected_components.hpp"
#include "util/timer.hpp"

namespace algorithms {

inline int ceilDiv(int a, int b) {
  return (a + b - 1) / b;
}

void checkMPI(int ret) {
  assert(ret == MPI_SUCCESS);
}

// compute the next highest power of 2 of 32-bit v
unsigned int nextPowerOf2(unsigned int v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;

  return v;
}

graph::HookTree parallelMpiConnectedComponents(graph::Label n_nodes,
                                               std::vector<graph::Edge>& all_edges,
                                               int n_threads_per_node, double* computation_time,
                                               double* reduction_time) {
  // get MPI params
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  using graph::Label;
  Label n_edges = all_edges.size();

  // broadcast number of edges

  auto MPI_type = parallel::MPITypeMap<Label>::value();
  checkMPI(MPI_Bcast(&n_edges, sizeof(Label), MPI_CHAR, 0, MPI_COMM_WORLD));
  checkMPI(MPI_Bcast(&n_nodes, sizeof(Label), MPI_CHAR, 0, MPI_COMM_WORLD));

  // scatter edges to all nodes
  const int buff_size = ceilDiv(n_edges, comm_size);

  std::vector<graph::Edge> my_edges;

  if (comm_size > 1 && rank == comm_size - 1) {
    my_edges.resize(n_edges - (buff_size * (comm_size - 1)));
  }
  else {
    my_edges.resize(buff_size);
  }

  // scatter edges
  if (rank == 0) {
    for (int dest = 1; dest < comm_size; ++dest) {
      const std::size_t sent_edges =
          dest != comm_size - 1 ? buff_size : n_edges - (buff_size * (comm_size - 1));
      checkMPI(MPI_Send(all_edges.data() + buff_size * dest, sent_edges * sizeof(graph::Edge),
                        MPI_CHAR, dest, 0, MPI_COMM_WORLD));
    }
    std::copy_n(all_edges.data(), my_edges.size(), my_edges.data());
  }
  else {
    checkMPI(MPI_Recv(my_edges.data(), my_edges.size() * sizeof(graph::Edge), MPI_CHAR, 0, 0,
                      MPI_COMM_WORLD, MPI_STATUS_IGNORE));
  }

  // Start the timer.
  MPI_Barrier(MPI_COMM_WORLD);
  const auto start_computation = util::getTime();

  // compute connected components
  graph::HookTree myHookTree =
      n_threads_per_node > 1 ? parallelConnectedComponents(n_nodes, my_edges, n_threads_per_node)
                             : serialConnectedComponents(n_nodes, my_edges);

  // combine results
  int n_active_nodes = nextPowerOf2(comm_size);
  constexpr int TAG_DATA = 20;
  bool done = false;
  std::vector<graph::Label> peer_parents;

  const auto start_redu = util::getTime(); // Note: for accurate timing insert a barrier.

  while (n_active_nodes > 1 && !done) {
    if (rank < n_active_nodes / 2) {
      // we are the receiver
      int peer_rank = rank + n_active_nodes / 2;
      if (peer_rank < comm_size) {
        peer_parents.resize(n_nodes);
        MPI_Recv(peer_parents.data(), n_nodes, MPI_type, peer_rank, TAG_DATA, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        graph::HookTree peerHookTree(std::move(peer_parents));
        myHookTree += peerHookTree;
        myHookTree.compress();
      }
    }
    else {
      // we are the sender
      int peer_rank = rank - n_active_nodes / 2;
      MPI_Send(myHookTree.getParents().data(), n_nodes, MPI_type, peer_rank, TAG_DATA,
               MPI_COMM_WORLD);
      done = true;
    }
    n_active_nodes = n_active_nodes / 2;
  }

  const auto end = util::getTime();

  if (computation_time)
    *computation_time = util::getDiff(start_computation, end);
  if (reduction_time)
    *reduction_time = util::getDiff(start_redu, end);

  return myHookTree;
}

}  // algorithms
