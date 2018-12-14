#include <iostream>
#include <fstream>
#include <functional>
#include <thread>
#include <random>

#include "gtest/gtest.h"
#include "testing/minimalist_printer.hpp"

#include "algorithms/node_distributed_connected_components.hpp"
#include "algorithms/parallel_mpi_connected_components.hpp"
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

TEST(NodeDistributedTest, Confront) {
  parallel::MpiConcurrency concurrency(0, nullptr, MPI_THREAD_MULTIPLE);
  using graph::Label;

  const int n_threads = 3;
  const Label size_x = 1000;
  const Label size_y = 1000;
  const double edge_probability = 0.5;

  const std::array<Label, 2> n_tiles_per_dim = util::partition(size_x, size_y, concurrency.size());

  if (concurrency.id() == 0) {
    std::cout << "Processes: " << n_tiles_per_dim[0] * n_tiles_per_dim[1]
              << " Threads: " << n_threads << std::endl;
    std::cout << "Nodes: " << size_x * size_y << std::endl;
  }

  Rng rng(0);

  graph::GridGraph grid(std::array<Label, 2>{size_x, size_y}, n_tiles_per_dim, edge_probability, rng);

  double compute_time;
  double total_time;

  auto forest = algorithms::gatherTree(
      algorithms::nodeDistributedConnectedComponents(grid.get_nodes(), grid.get_edges(), n_threads,
                                                     &compute_time, &total_time),
      grid.get_nodes());

  double compute_time2, total_time2;
  auto edge_copy = grid.get_edges();
  auto confront = algorithms::parallelMpiConnectedComponents(grid.get_nodes(), edge_copy, n_threads,
                                                             &compute_time2, &total_time2);

  if (concurrency.id() == 0) {
    ASSERT_EQ(forest.size(), confront.size());

    for (int i = 0; i < confront.size(); ++i) {
      EXPECT_EQ(forest.parent(i), confront.parent(i));
    }
  }
}
