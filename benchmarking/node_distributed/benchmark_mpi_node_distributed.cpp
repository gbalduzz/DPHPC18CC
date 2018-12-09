#include <iostream>
#include <fstream>
#include <functional>
#include <thread>
#include <random>

#include "algorithms/node_distributed_connected_components.hpp"
#include "graph/grid_graph.hpp"
#include "parallel/concurrency/mpi_concurrency.hpp"
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
  parallel::MpiConcurrency concurrency(argc, argv);

  if (argc <= 3) {
    std::cout << "Usage: $0 threads linear_grid_size probability [output file]\n";
    std::exit(-5);
  }

  const int n_threads = std::atoi(argv[1]);
  const int grid_linsize = std::atoi(argv[2]);
  const double edge_probability = std::atof(argv[3]);

  const std::string filename = argc > 4 ? std::string(argv[4]) : "distributed_times.txt";

  if (concurrency.id() == 0) {
    std::cout << "Processes:" << concurrency.size() << " Threads:" << n_threads << std::endl;
    std::cout << "Nodes:" << grid_linsize * grid_linsize << std::endl;
  }

  Rng rng(0);
  const int n_tiles_per_dim = std::sqrt(concurrency.size());
  if (n_tiles_per_dim * n_tiles_per_dim != concurrency.size()) {
    std::cout << "Number of ranks is not a square.\n";
    std::exit(-1);
  }

  graph::GridGraph grid(grid_linsize, n_tiles_per_dim, edge_probability, rng);

  if (concurrency.id() == 0)
    std::cout << "Generated " << grid.get_edges().size() << " edges.\n";

  constexpr int n_times = 10;
  std::vector<double> results(n_times);

  std::ofstream out;
  if (concurrency.id() == 0) {
    out.open(filename);
    out << "# threads " << n_threads << " processes " << concurrency.size() << " vertices "
        << grid.get_nodes() << " edges " << grid.get_edges().size() << "\n"
        << "# algorithm_time \t total_time\n";
  }

  for (auto& result : results) {
    std::vector<graph::Edge> edge_copy(grid.get_edges());
    double compute_time;
    double total_time;
    auto ret = algorithms::nodeDistributedConnectedComponents(
        grid.get_nodes(), edge_copy, n_threads, &compute_time, &total_time);
    result = compute_time;
    if (concurrency.id() == 0)
      out << compute_time << "\t" << total_time << "\n";
  }

  if (concurrency.id() == 0) {
    const double mean = util::avg(results);
    const double err = util::stddev(results) / std::sqrt(n_times);
    std::cout << "Time: " << mean << " +- " << err << std::endl;
  }

  return 0;
}
