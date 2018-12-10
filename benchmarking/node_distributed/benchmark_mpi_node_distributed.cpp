#include <iostream>
#include <fstream>
#include <functional>
#include <thread>
#include <random>

#include "algorithms/node_distributed_connected_components.hpp"
#include "graph/grid_graph.hpp"
#include "parallel/concurrency/mpi_concurrency.hpp"
#include "util/partition.hpp"
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
  parallel::MpiConcurrency concurrency(argc, argv, MPI_THREAD_MULTIPLE);

  if (argc <= 4) {
    if (concurrency.id() == 0)
      std::cout << "Usage: $0 threads grid_size _x grid_size_y probability [output file]\n";
    std::exit(-5);
  }

  const int n_threads = std::atoi(argv[1]);
  const int size_x = std::atoi(argv[2]);
  const int size_y = std::atoi(argv[3]);
  const double edge_probability = std::atof(argv[4]);

  const std::string filename = argc > 5 ? std::string(argv[5]) : "distributed_times.txt";

  const std::array<int, 2> n_tiles_per_dim = util::partition(size_x, size_y, concurrency.size());

  if (n_tiles_per_dim[0] * n_tiles_per_dim[1] != concurrency.size()) {
    if (concurrency.id() == 0)
      std::cout << "Tiling failed.\n";
    std::exit(-1);
  }

  if (concurrency.id() == 0) {
    std::cout << "Processes: " << n_tiles_per_dim[0] * n_tiles_per_dim[1]
              << " Threads: " << n_threads << std::endl;
    std::cout << "Nodes: " << size_x * size_y << std::endl;
  }

  Rng rng(0);

  graph::GridGraph grid(std::array<int, 2>{size_x, size_y}, n_tiles_per_dim, edge_probability, rng);

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
