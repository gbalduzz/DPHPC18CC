#include <iostream>
#include <fstream>
#include <functional>
#include <thread>

#include "algorithms/parallel_connected_components.hpp"
#include "algorithms/serial_connected_components.hpp"
#include "graph/generate_random_graph.hpp"
#include "util/timer.hpp"
#include "util/stddev.hpp"

std::ostream& operator<<(std::ostream& out, std::pair<double, double> p) {
  return out << p.first << "\t" << p.second;
}

int main(int argc, char** argv) {
  int max_threads = std::thread::hardware_concurrency();
  if (argc > 1)
    max_threads = std::atoi(argv[1]);

  constexpr int n = 1e7;
  constexpr int m = 1e7;

  const auto edges = graph::generateRandomGraph(n, m);
  std::cout << "Generated random graph.\n";

  auto time = [&](std::function<void()>&& f) -> std::pair<double, double> {
    constexpr int n_times = 10;
    std::vector<double> results(n_times);

    for (auto& result : results) {
      const auto start = util::getTime();
      f();
      auto end = util::getTime();
      result = util::getDiff(start, end);
    }

    const double mean = util::avg(results);
    const double err = util::stddev(results) / std::sqrt(n_times);
    return std::make_pair(mean, err);
  };

  std::ofstream out("result.txt");
  out << "# n_threads(0 = serial) \t time \t err.\n";

  // Time serial algorithm.
  out << 0 << "\t" << time(std::bind(algorithms::serialConnectedComponents, n, edges)) << std::endl;

  std::cout << "Done serial.\n";

  // Time parallel.
  for (int threads = 1; threads <= max_threads; ++threads) {
    out << threads << "\t"
        << time(std::bind(algorithms::parallelConnectedComponents, n, edges, threads)) << std::endl;

    std::cout << "Done parallel " << threads << ".\n";
  }

  out.close();

  return 0;
}
