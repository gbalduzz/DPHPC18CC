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
  const std::string filename = argc > 2 ? std::string(argv[2]) : "USA-road-t.NY.gr";
  const std::string output_filename =
      argc > 3 ? std::string(argv[3])
               : "benchmark_mpi_" + std::to_string(concurrency.size()) + ".txt";
  const int n_repetitions = argc > 4 ? std::atoi(argv[4]) : 10;

  if (concurrency.id() == 0) {
    std::cout << "Processes:" << concurrency.size() << " Threads:" << n_threads << std::endl;
  }

  std::vector<graph::Edge> edges;
  graph::Label n;
  std::ofstream out(output_filename);

  if (concurrency.id() == 0) {
    util::GraphReader::readCommAvoidingInput(filename, edges, n);
    std::cout << "Loaded graph.\n";
  }
  concurrency.broadcast(n);
  std::vector<double> results(n_repetitions);

  if (concurrency.id() == 0) {
    out << "# threads " << n_threads << " processes " << concurrency.size() << " vertices " << n
        << " edges " << edges.size() << "\n"
        << "# algorithm_time \t total_time\n";
  }

  for (auto& result : results) {
    double compute_time, total_time;
    auto ret =
        algorithms::parallelMpiConnectedComponents(n, edges, n_threads, &compute_time, &total_time);
    result = compute_time;
    if (concurrency.id() == 0) {
      out << compute_time << "\t" << total_time << "\n";
    }
  }
  out.close();

  if (concurrency.id() == 0) {
    const double mean = util::avg(results);
    const double err = util::stddev(results) / std::sqrt(n_repetitions);
    std::cout << "Time: " << mean << " +- " << err << std::endl;
  }

  return 0;
}
