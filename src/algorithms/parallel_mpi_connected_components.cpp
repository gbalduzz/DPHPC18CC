#include "algorithms/parallel_mpi_connected_components.hpp"
#include "algorithms/parallel_connected_components.hpp"
#include "util/timer.hpp"

#include <mpi.h>

#include <omp.h>
#include <math.h>

namespace algorithms {

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
  MPI_Bcast(&n_edges, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // printf("P_%d: n_edges = %d\n", rank, n_edges);

  // Start the timer.
  MPI_Barrier(MPI_COMM_WORLD);
  const auto start_computation = util::getTime();

  // scatter edges to all nodes

  int buff_size = 2.0f * std::ceil((float)n_edges / comm_size);
  int* sendbuff = nullptr;
  int recvbuff[buff_size];
  if (rank == 0) {
    printf("start scattering\n");

    // create send buffer
    int sendbuffsize = comm_size * buff_size;
    sendbuff = new int[sendbuffsize];

    // write edges into send buffer
    for (int i = 0; i < n_edges; ++i) {
      sendbuff[2 * i + 0] = all_edges[i].first;
      sendbuff[2 * i + 1] = all_edges[i].second;

      if (sendbuff[2 * i + 0] == -1 or sendbuff[2 * i + 1] == -1) {
        printf("wrong edge written to sendbuff\n");
      }
    }

    // padd send buffer with -1
    for (int i = (2 * n_edges); i < sendbuffsize; ++i) {
      sendbuff[i] = -1;
    }
  }

  // scatter edges
  MPI_Scatter(sendbuff, buff_size, MPI_INT, recvbuff, buff_size, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("finished scattering\n");
  }

  // assemble edges from recvbuffer
  int n_my_edges = buff_size / 2;
  int n_my_nodes = 0;
  std::vector<graph::Edge> my_edges;
  for (int i = 0; i < n_my_edges; ++i) {
    graph::Edge e(recvbuff[2 * i + 0], recvbuff[2 * i + 1]);

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

  // compute connected components
  printf("P_%d: start parallel contractions\n", rank);
  graph::HookTree myHookTree = parallelConnectedComponents(n_my_nodes, my_edges, n_threads_per_node);
  printf("P_%d: end parallel contractions\n", rank);

  // combine results
  int n_active_nodes = comm_size;
  const int TAG_N_NODES = 10;
  const int TAG_DATA = 20;
  bool done = false;
  while (n_active_nodes > 1) {
    // if there is an odd number of active nodes the root node skipps this step
    if (not(rank == 0 && n_active_nodes % 2 == 1) && !done) {
      if (rank < (float)n_active_nodes / 2) {
        // we are the receiver
        int peer_rank = rank + floor((float)n_active_nodes / 2);
        int n_peer_nodes;
        MPI_Recv(&n_peer_nodes, 1, MPI_INT, peer_rank, TAG_N_NODES, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        unsigned peer_parents[n_peer_nodes];
        MPI_Recv(peer_parents, n_peer_nodes, MPI_UNSIGNED, peer_rank, TAG_DATA, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        graph::HookTree peerHookTree(peer_parents, n_peer_nodes);

        myHookTree += peerHookTree;
        myHookTree.compress();
      }
      else {
        // we are the sender
        int peer_rank = rank - floor((float)n_active_nodes / 2);
        MPI_Send(&n_my_nodes, 1, MPI_INT, peer_rank, TAG_N_NODES, MPI_COMM_WORLD);
        MPI_Send(myHookTree.getParents().data(), n_my_nodes, MPI_UNSIGNED, peer_rank, TAG_DATA,
                 MPI_COMM_WORLD);
        done = true;
        printf("P_%d: done\n", rank);
      }
    }

    n_active_nodes = (int)ceil((float)n_active_nodes / 2);
  }

  const auto end = util::getTime();

  if (computation_time)
    *computation_time = util::getDiff(start_computation, end);
  if (total_time)
    *total_time = util::getDiff(start, end);

  return myHookTree;
}

}  // algorithms
