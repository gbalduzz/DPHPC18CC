#include <iostream>
#include <fstream>
#include <functional>
#include <thread>
#include <random>

#include "algorithms/serial_connected_components.hpp"
#include "graph/grid_graph.hpp"
#include "graph/connected_components_analysis.hpp"
#include "util/timer.hpp"
#include "util/stddev.hpp"

std::ostream& operator<<(std::ostream& out, std::pair<double, double> p) {
  return out << p.first << "\t" << p.second;
}

class Rng {
public:
  Rng(int seed = 0) : rng_(seed), distro_(0, 1) {}

  double operator()() {
    return distro_(rng_);
  }

private:
  std::mt19937_64 rng_;
  std::uniform_real_distribution<double> distro_;
};

int main(int argc, char** argv) {
  const double edge_probability = argc > 1 ? std::atof(argv[1]) : 0.25;
  const int nx = argc > 2 ? std::atoi(argv[2]) : 10000;

  std::cout << "Running with prob " << edge_probability << " and x size " << nx << std::endl;

  Rng rng(0);

  for (int run = 1; run <= 10; ++run) {
    const int ny = nx  * run;
    std::ofstream out("run_" + std::to_string(run) + "_timings.txt");
    out << "#  vertices " << nx * ny << "\n"
        << "# algorithm_time \t edges \t connected_comps \t avg_size \n";

    constexpr int n_times = 10;
    std::vector<double> results(n_times);

    using graph::Label ;
    for (int count = 0; count < n_times; ++count) {
      graph::GridGraph grid(std::array<Label, 2>{nx, ny}, std::array<Label, 2>{1, 1},
                            edge_probability, rng);

      std::size_t n_edges = grid.get_edges().size();
      std::cout << "Generated " << n_edges << " edges.\n";

      const auto start = util::getTime();
      auto tree = algorithms::serialConnectedComponents(grid.get_nodes(), grid.get_edges());
      const auto end = util::getTime();

      const double time = util::getDiff(start, end);

      auto components = graph::connectedComponentsSize(tree);

      out << time << "\t" << grid.get_edges().size() << "\t" << components.size() << "\t"
          << util::avg(components) << std::endl;
      results.push_back(time);
    }

    const double mean = util::avg(results);
    const double err = util::stddev(results) / std::sqrt(n_times);
    std::cout << "Time: " << mean << " +- " << err << std::endl;
  }

  return 0;
}
