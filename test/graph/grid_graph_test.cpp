#include "graph/grid_graph.hpp"

#include <gtest/gtest.h>

struct MockRng {
  MockRng(const std::vector<double>& values) : values_(values) {}

  double operator()() {
    if (index_ == values_.size())
      throw(std::out_of_range("No more values."));

    double val = values_[index_++];
    return val;
  }

  std::vector<double> values_;
  int index_ = 0;
};

TEST(GridGraphTest, All) {
  std::vector<double> vals(16 * 2, 1);
  vals[0] = 0; // 0-1
  vals[2] = 0; // 1-4
  vals[8] = 0; // 2-8

  MockRng rng{vals};
  graph::GridGraph grid(4, 2, 0.5, rng);

  EXPECT_EQ(grid.get_nodes(), 16);

  const auto edges = grid.get_edges();
  EXPECT_EQ(edges.size(), 3);

  EXPECT_EQ(edges[0], graph::Edge(0, 1));
  EXPECT_EQ(edges[1], graph::Edge(1, 4));
  EXPECT_EQ(edges[2], graph::Edge(2, 8));
}
