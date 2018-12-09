// This class generates a graph representing a 2D grid with nearest neighbour connections.
// The nodes are Morton ordered.

#pragma once

#include <array>
#include <limits>
#include <vector>
#include <stdexcept>

#include "graph/edge.hpp"
#include "util/ceil_div.hpp"

namespace graph {

class GridGraph {
public:
  template <class Rng>
  GridGraph(std::array<int, 2> grid_length, std::array<int, 2> n_tiles, double prob_connection,
            Rng& rng);

  template <class Rng>
  GridGraph(int grid_length, int n_tiles, double prob_connection, Rng& rng);

  const std::vector<Edge>& get_edges() const {
    return edges_;
  }

  Label get_nodes() const {
    return nodes_;
  }

private:
  Label coordinatesToId(Label x, Label y) const;

  const std::array<int, 2> grid_size_;
  const std::array<int, 2> tile_size_;
  const std::array<int, 2> n_tiles_per_dim_;

  std::vector<Edge> edges_;
  const Label nodes_;
};

template <class Rng>
GridGraph::GridGraph(int grid_length, int n_tiles, double prob_connection, Rng& rng)
    : GridGraph(std::array<int, 2>{grid_length, grid_length}, std::array<int, 2>{n_tiles, n_tiles},
                prob_connection, rng) {}

template <class Rng>
GridGraph::GridGraph(std::array<int, 2> grid_size, std::array<int, 2> n_tiles,
                     double prob_connection, Rng& rng)
    : grid_size_(grid_size),
      n_tiles_per_dim_(n_tiles),
      tile_size_{util::ceilDiv(grid_size[0], n_tiles[0]), util::ceilDiv(grid_size[1], n_tiles[1])},
      nodes_(grid_size_[0] * grid_size_[1]) {
  if (grid_size[0] >= (std::numeric_limits<Label>::max() - 1) / grid_size[1])
    throw(std::out_of_range("Use a bigger representation."));

  auto generate_connection = [&](Label x1, Label y1, Label x2, Label y2) {
    if (rng() < prob_connection) {  // Insert an edge with probability prob connection.
      edges_.push_back(Edge(coordinatesToId(x1, y1), coordinatesToId(x2, y2)));
    }
  };

  for (Label y = 0; y < grid_size[1]; ++y)
    for (Label x = 0; x < grid_size[0]; ++x) {
      if (x + 1 < grid_size[0])
        generate_connection(x, y, x + 1, y);
      if (y + 1 < grid_size[1])
        generate_connection(x, y, x, y + 1);
    }
}

inline Label GridGraph::coordinatesToId(Label x, Label y) const {
  const Label tile_x = x / tile_size_[0];
  const Label tile_y = y / tile_size_[1];

  const Label tile_start =
      tile_x * tile_size_[0] * tile_size_[1] + tile_y * grid_size_[0] * tile_size_[1];

  // Coordinates relative to the tile start.
  x -= tile_x * tile_size_[0];
  y -= tile_y * tile_size_[1];

  return tile_start + x + y * tile_size_[0];
}

}  // graph
