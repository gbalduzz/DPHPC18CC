#include <iostream>
#include <fstream>
#include <functional>
#include <thread>

#include "algorithms/parallel_connected_components.hpp"
#include "algorithms/serial_connected_components.hpp"
#include "util/graph_reader.hpp"
#include "util/timer.hpp"
#include "util/stddev.hpp"

// std::ostream& operator<<(std::ostream& out, std::pair<double, double> p) {
//  return out << p.first << "\t" << p.second;
//}

int main(int argc, char** argv) {
  const int max_threads = argc > 1 ? std::atoi(argv[1]) : 1;
  const std::string filename = argc > 2 ? std::string(argv[2]) : "USA-road-t.NY.gr";
  const std::string output_prename = argc > 3 ? std::string(argv[3]) : "benchmark_omp";
  const int n_repetitions = argc > 4 ? std::atoi(argv[4]) : 10;

  std::vector<graph::Edge> edges;
  graph::Label n;

  util::GraphReader::readCommAvoidingInput(filename, edges, n);
  std::cout << "Loaded graph.\n";

  for (int threads = 1; threads < max_threads + 1; ++threads) {
    std::cout << "Threads:" << threads << std::endl;

    const std::string outname = output_prename + "_" + std::to_string(threads) + ".txt";
    std::ofstream out(outname);
    std::vector<double> results(n_repetitions);

    out << "# threads " << threads << " vertices " << n << " edges " << edges.size() << "\n"
        << "# algorithm_time\n";

    for (auto& result : results) {
      graph::HookTree tree(0);

      const auto start = util::getTime();
      if (threads > 1)
        tree = std::move(algorithms::parallelConnectedComponents(n, edges, threads));
      else
        tree = std::move(algorithms::serialConnectedComponents(n, edges));
      const auto end = util::getTime();

      if(n != tree.size())
          throw(std::logic_error("Something went wrong."));

      const double time = util::getDiff(start, end);
      out << time  << "\n";
      result = time;
    }
    out.close();
    const double mean = util::avg(results);
    const double err = util::stddev(results) / std::sqrt(n_repetitions);
    std::cout << "Time: " << mean << " +- " << err << std::endl;
  }
}
