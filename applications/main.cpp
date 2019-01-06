#include <iostream>
#include <fstream>
#include <mpi.h>
#include <parallel/concurrency/mpi_type_map.hpp>

#include "algorithms/parallel_connected_components.hpp"
#include "algorithms/parallel_mpi_connected_components.hpp"
#include "algorithms/serial_connected_components.hpp"
#include "graph/generate_random_graph.hpp"
#include "io/json11.hpp"
#include "util/timer.hpp"
#include "util/graph_reader.hpp"

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<graph::Edge> edges;
  graph::Label n_nodes;

  if (rank == 0) {
    std::string filename = "../benchmarking/USA-road-t.NY.gr";

    printf("start load graph\n");
    edges = util::GraphReader().read_graph_from_DIMACS_challenge(filename);
    printf("end load graph\n");

    n_nodes = util::GraphReader().vertexNumber(edges);

    graph::HookTree serialHookTree = algorithms::serialConnectedComponents(n_nodes, edges);
    printf("Serial number of connected componnets: %d\n", serialHookTree.getNumConnectedComponents());

    graph::HookTree parallelHookTree = algorithms::parallelConnectedComponents(n_nodes, edges, 4);
    printf("Parallel number of connected componnets: %d\n",
           parallelHookTree.getNumConnectedComponents());
  }

  MPI_Bcast(&n_nodes, 1, parallel::MPITypeMap<graph::Label>::value(), 0, MPI_COMM_WORLD);
  graph::HookTree MpiHookTree = algorithms::parallelMpiConnectedComponents(n_nodes, edges, 4);

  if (rank == 0) {
    // printf("%s\n", finalHookTree.toString().c_str());
    printf("MPI number of connected components: %d\n", MpiHookTree.getNumConnectedComponents());
  }

  MPI_Finalize();

  /*
  std::string input_name = "input.json";
  if (argc > 1)
    input_name = std::string(argv[1]);

  json11::Json parameters = json11::Json::parseFile(input_name);

  const int n = parameters["vertices"].int_value();
  const int m = parameters["edges"].int_value();
  auto edges = graph::generateRandomGraph(n, m);
  std::cout << "Generated random graph.\n";

  const int n_threads = parameters["threads"].int_value();

  auto start = util::getTime();
  auto result = algorithms::parallelConnectedComponents(n, edges, n_threads);
  auto end = util::getTime();

  std::cout << "Parallel contraction took " << util::getDiff(start, end) << " s" << std::endl;
  std::cout << "N threads " << n_threads << std::endl;

  std::ofstream out("labels.txt");
  for (int i = 0; i < n; ++i)
    out << i << ":\t" << result.representative(i) << "\n";
  out.close();
  */

  return 0;
}
