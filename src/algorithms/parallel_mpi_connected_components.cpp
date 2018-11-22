#include "algorithms/parallel_mpi_connected_components.hpp"

#include <cmath>
#include <vector>

#include <mpi.h>
#include <omp.h>

#include "algorithms/parallel_connected_components.hpp"
#include "util/timer.hpp"

namespace algorithms {

inline int ceilDiv(int a, int b) {
  return (a + b - 1) / b;
}

graph::HookTree parallelMpiConnectedComponents(std::vector<graph::Edge>& all_edges,
                                               int n_threads_per_node, double* computation_time,
                                               double* total_time) {
  const auto start = util::getTime();

  // get MPI params
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  int n_edges = -1;

  if (rank == 0) {
    n_edges = all_edges.size();
  }

  // broadcast number of edges
  int ret;
  ret = MPI_Bcast(&n_edges, 1, MPI_INT, 0, MPI_COMM_WORLD);

  assert(ret == MPI_SUCCESS);
  // printf("P_%d: n_edges = %d\n", rank, n_edges);

  // scatter edges to all nodes
  const int buff_size = ceilDiv(n_edges, comm_size);
  std::vector<graph::Edge> recvbuff(buff_size);
  if (rank == 0) {
    printf("start scattering\n");

    // Pad the edges with -1.
    int sendbuffsize = comm_size * buff_size;
    auto old_end = all_edges.end();
    all_edges.resize(sendbuffsize);
    std::fill(old_end, all_edges.end(), graph::Edge(-1, -1));
  }

  // scatter edges
  // TODO : consider an MPI custom type.
  MPI_Scatter(all_edges.data(), 2 * buff_size, MPI_UNSIGNED, recvbuff.data(), 2 * buff_size,
              MPI_UNSIGNED, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("finished scattering\n");
  }

  // TODO: avoid copy.
  // assemble edges from recvbuffer
  int n_my_edges = buff_size / 2;
  int n_my_nodes = 0;
  std::vector<graph::Edge> my_edges;
  for (const auto& e : recvbuff) {
    if (e.first != -1 and e.second != -1) {
      if (n_my_nodes < e.first || n_my_nodes < e.second) {
        n_my_nodes = std::max(e.first, e.second);
      }
      my_edges.push_back(e);
    }
    else if (e.first != -1 or e.second != -1) {
      printf("P_%d: Invalid edge %d - %d detected\n", rank, e.first, e.second);
    }
  }
  n_my_nodes++;  // the number of nodes is one more than the maximum node id

  // Start the timer.
  MPI_Barrier(MPI_COMM_WORLD);
  const auto start_computation = util::getTime();

  // compute connected components
  printf("P_%d: start parallel contractions\n", rank);
  graph::HookTree myHookTree = parallelConnectedComponents(n_my_nodes, my_edges, n_threads_per_node);
  printf("P_%d: end parallel contractions\n", rank);

  // combine results
  int n_active_nodes = comm_size;
  const int TAG_N_NODES = 10;
  const int TAG_DATA = 20;
  bool done = false;
  while (n_active_nodes > 1 && !done) {
    if (rank < n_active_nodes / 2) {
      // we are the receiver
      int peer_rank = rank + n_active_nodes / 2;
      int n_peer_nodes;
      MPI_Recv(&n_peer_nodes, 1, MPI_INT, peer_rank, TAG_N_NODES, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      std::vector<graph::Label> peer_parents(n_peer_nodes);
      MPI_Recv(peer_parents.data(), n_peer_nodes, MPI_UNSIGNED, peer_rank, TAG_DATA, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      graph::HookTree peerHookTree(std::move(peer_parents));

      myHookTree += peerHookTree;
      myHookTree.compress();
    }
    else {
      // we are the sender
      int peer_rank = rank - n_active_nodes / 2;
      if (peer_rank < 0)  // In case of odd number of ranks, there could be no process to receive.
        continue;
      MPI_Send(&n_my_nodes, 1, MPI_INT, peer_rank, TAG_N_NODES, MPI_COMM_WORLD);
      MPI_Send(myHookTree.getParents().data(), n_my_nodes, MPI_UNSIGNED, peer_rank, TAG_DATA,
               MPI_COMM_WORLD);
      done = true;
      printf("P_%d: done\n", rank);
    }
    n_active_nodes = ceilDiv(n_active_nodes, 2);
  }

  const auto end = util::getTime();

  if (computation_time)
    *computation_time = util::getDiff(start_computation, end);
  if (total_time)
    *total_time = util::getDiff(start, end);

  return myHookTree;
}

}  // algorithms
