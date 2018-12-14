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

graph::HookTree parallelMpiConnectedComponents(graph::Label n_nodes, std::vector<graph::Edge>& all_edges,
                                               int n_threads_per_node, double* computation_time,
                                               double* total_time) {
  const auto start = util::getTime();

  // get MPI params
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  using graph::Label;
  Label n_edges = all_edges.size();

  // broadcast number of edges

  auto MPI_type = parallel::MPITypeMap<Label>::value();
  checkMPI(MPI_Bcast(&n_edges, 1, MPI_type, 0, MPI_COMM_WORLD));

  // scatter edges to all nodes
  const int buff_size = ceilDiv(n_edges, comm_size);
  std::vector<graph::Edge> my_edges(buff_size);
  if (rank == 0) {
    //      printf("start scattering\n");

    // Pad the edges with -1.
    int sendbuffsize = comm_size * buff_size;
    all_edges.resize(sendbuffsize);
    std::fill(all_edges.begin() + n_edges, all_edges.end(), graph::Edge(-1, -1));
  }

  // scatter edges
  // TODO : consider an MPI custom type.
  MPI_Scatter(all_edges.data(), 2 * buff_size, MPI_type, my_edges.data(), 2 * buff_size,
              MPI_type, 0, MPI_COMM_WORLD);

  // if (rank == 0) {
  //  printf("finished scattering\n");
  //}

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
      //      printf("P_%d: done\n", rank);
    }
    n_active_nodes = n_active_nodes / 2;
  }

  const auto end = util::getTime();

  if (computation_time)
    *computation_time = util::getDiff(start_computation, end);
  if (total_time)
    *total_time = util::getDiff(start, end);

  return myHookTree;
}

}  // algorithms
