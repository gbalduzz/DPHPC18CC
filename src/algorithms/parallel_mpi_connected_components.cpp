#include "algorithms/parallel_mpi_connected_components.hpp"
#include "algorithms/parallel_connected_components.hpp"
#include <mpi.h>

#include <omp.h>
#include <math.h>

#include "util/graph_reader.hpp"

namespace algorithms {


    graph::HookTree parallelMpiConnectedComponents(std::string graph_file_name, int n_threads_per_node) {



        // get MPI params
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        int comm_size;
        MPI_Comm_size(MPI_COMM_WORLD, &comm_size);


        // load edges from file on root node
        int n_edges = -1;
        std::vector<graph::Edge> all_edges;
        if(rank == 0) {
            // read edges from file
            util::GraphReader graphReader;
            all_edges = graphReader.read_graph_from_adjacency_list(graph_file_name);
            n_edges = all_edges.size();
        }


        // broadcast number of edges
        MPI_Bcast(&n_edges, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("P_%d: n_edges = %d\n", rank, n_edges);






        // scatter edges to all nodes

        int buff_size = 2.0f*std::ceil((float)n_edges / comm_size);
        int* sendbuff = nullptr;
        int recvbuff[buff_size];
        if(rank == 0) {

            // create send buffer
            int sendbuffsize = comm_size * buff_size;
            sendbuff = new int[sendbuffsize];

            // write edges into send buffer
            for(int i=0; i<n_edges; ++i) {
                sendbuff[2*i + 0] = all_edges[i].first;
                sendbuff[2*i + 1] = all_edges[i].second;
            }

            // padd send buffer with -1
            for(int i=(2*n_edges); i<sendbuffsize; ++i) {
                sendbuff[i] = -1;
            }
        }


        // scatter edges
        MPI_Scatter(sendbuff, buff_size, MPI_INT, recvbuff, buff_size, MPI_INT, 0, MPI_COMM_WORLD);




        // assemble edges from recvbuffer
        int n_my_edges = buff_size / 2;
        int n_my_nodes = 0;
        std::vector<graph::Edge> my_edges;
        for(int i=0; i<n_my_edges; ++i) {
            graph::Edge e(recvbuff[2*i+0], recvbuff[2*i+1]);

            if(e.first != -1 and e.second != -1) {
                if(n_my_nodes < e.first || n_my_nodes < e.second) {
                    n_my_nodes = std::max(e.first, e.second);
                }
                my_edges.push_back(e);
            }
            else if(e.first != -1 or e.second != -1) {
                printf("P_%d: Invalid edge %d - %d detected\n", rank, e.first, e.second);
            }
        }
        n_my_nodes++;

        printf("P_%d: num nodes = %d\n", rank, n_my_nodes);
        for(int i=0; i<my_edges.size(); ++i) {
            printf("P_%d: %d - %d\n", rank, my_edges[i].first, my_edges[i].second);
        }





        // compute connected components
        graph::HookTree myHookTree = parallelConnectedComponents(n_my_nodes, my_edges, n_threads_per_node);
        MPI_Barrier(MPI_COMM_WORLD);


        for(int i=0; i<comm_size; ++i) {
            if(rank == i) {
                printf("P_%d:\n%s", rank, myHookTree.toString().c_str());
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }



        return graph::HookTree(0);

    }



}  // algorithms