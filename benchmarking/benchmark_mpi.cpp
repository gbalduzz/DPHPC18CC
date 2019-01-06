#include <iostream>
#include <fstream>
#include <functional>
#include <thread>

#include "algorithms/parallel_mpi_connected_components.hpp"
#include "parallel/concurrency/mpi_concurrency.hpp"
#include "util/graph_reader.hpp"
#include "util/timer.hpp"
#include "util/stddev.hpp"

std::ostream& operator<<(std::ostream& out, std::pair<double, double> p) {
  return out << p.first << "\t" << p.second;
}

int main(int argc, char** argv) {
  parallel::MpiConcurrency concurrency(argc, argv);

  const int n_threads = argc > 1 ? std::atoi(argv[1]) : 1;

  if (concurrency.id() == 0) {
    std::cout << "Nodes:" << concurrency.size() << " Threads:" << n_threads << std::endl;
  }
  const std::string filename = argc > 2 ? std::string(argv[2]) : "rmat_1024k_128.in";

  std::vector<graph::Edge> edges;
  graph::Label n;

  if (concurrency.id() == 0) {
    edges = util::GraphReader().read_graph_from_paramat(filename);
    n = util::GraphReader().vertexNumber(edges);
    // std::cout << "Loaded graph.\n";
  }
  concurrency.broadcast(n);

  constexpr int n_times = 10;
  std::vector<double> results(n_times);

  for (auto& result : results) {
    std::vector<graph::Edge> edge_copy(edges);
    double compute_time;
    auto ret = algorithms::parallelMpiConnectedComponents(n, edge_copy, n_threads, &compute_time);
    result = compute_time;
  }

  if (concurrency.id() == 0) {
    const double mean = util::avg(results);
    const double err = util::stddev(results) / std::sqrt(n_times);
    std::cout << "Time: " << mean << " +- " << err << std::endl;
  }

  return 0;
}
