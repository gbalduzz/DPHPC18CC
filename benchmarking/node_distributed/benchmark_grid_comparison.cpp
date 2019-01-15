#include <iostream>
#include <fstream>
#include <functional>
#include <thread>
#include <random>

#include "algorithms/node_distributed_connected_components.hpp"
#include "algorithms/parallel_mpi_connected_components.hpp"
#include "graph/grid_graph_distributed.hpp"
#include "parallel/concurrency/mpi_concurrency.hpp"
#include "util/partition.hpp"
#include "util/timer.hpp"
#include "util/stddev.hpp"

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
  using graph::Label;
  parallel::MpiConcurrency concurrency(argc, argv, MPI_THREAD_MULTIPLE);

  if (argc <= 6) {
    if (concurrency.id() == 0)
      std::cout
          << "Usage: $0 threads grid_size _x grid_size_y probability output_prename n_times\n";
    std::exit(-5);
  }

  const int n_threads = std::atoi(argv[1]);
  const Label size_x = std::atoi(argv[2]);
  const Label size_y = std::atoi(argv[3]);
  const double edge_probability = std::atof(argv[4]);
  const std::string filename = std::string(argv[5]);
  const int n_times = std::atoi(argv[6]);

  const std::array<Label, 2> n_tiles_per_dim = util::partition(size_x, size_y, concurrency.size());

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

  Rng rng(concurrency.id());

  graph::GridGraph grid(std::array<Label, 2>{size_x, size_y}, n_tiles_per_dim, edge_probability, rng);
  const std::size_t n_edges = grid.get_edges().size();

  if (concurrency.id() == 0)
    std::cout << "Generated " << n_edges << " edges.\n";

  std::vector<double> results(n_times);

  {  // Distributed
    std::ofstream out;
    if (concurrency.id() == 0) {
      std::cout << "Starting Distributed algorithm.\n";

      out.open(filename + "_t_" + std::to_string(n_threads) + "_distributed.txt");
      out << "# threads " << n_threads << " processes " << concurrency.size() << " vertices "
          << grid.get_nodes() << " edges " << n_edges << "\n"
          << "# algorithm_time \t total_time\n";
    }

    for (auto& result : results) {
      double compute_time;
      double total_time;
      auto ret = algorithms::nodeDistributedConnectedComponents(
          grid.get_nodes(), grid.get_edges(), n_threads, &compute_time, &total_time);
      result = compute_time;
      if (concurrency.id() == 0)
        out << compute_time << "\t" << total_time << "\n";
    }

    if (concurrency.id() == 0) {
      const double mean = util::avg(results);
      const double err = util::stddev(results) / std::sqrt(n_times);
      std::cout << "Time: " << mean << " +- " << err << std::endl;
    }
  }

  {  // Reduction
    std::ofstream out;
    if (concurrency.id() == 0) {
      std::cout << "Starting Reduction algorithm.\n";

      out.open(filename + "_t_" + std::to_string(n_threads) + "_reduction.txt");
      out << "# threads " << n_threads << " processes " << concurrency.size() << " vertices "
          << grid.get_nodes() << " edges " << n_edges << "\n"
          << "# algorithm_time \t reduction_time\n";
    }

    for (auto& result : results) {
      double compute_time;
      double reduction_time;
      auto ret = algorithms::parallelMpiConnectedComponents(
          grid.get_nodes(), grid.get_edges(), n_threads, &compute_time, &reduction_time);
      result = compute_time;
      if (concurrency.id() == 0)
        out << compute_time << "\t" << reduction_time << "\n";
    }

    if (concurrency.id() == 0) {
      const double mean = util::avg(results);
      const double err = util::stddev(results) / std::sqrt(n_times);
      std::cout << "Time: " << mean << " +- " << err << std::endl;
    }
  }

  return 0;
}
