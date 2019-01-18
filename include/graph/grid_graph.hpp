// This class generates a graph representing a 2D grid with nearest neighbour connections.
// The nodes are Morton ordered.

#pragma once

#include <array>
#include <cassert>
#include <limits>
#include <vector>
#include <stdexcept>

#include "graph/edge.hpp"
#include "util/ceil_div.hpp"

namespace graph {

class GridGraph {
public:
  GridGraph(std::array<Label, 2> grid_length, std::array<Label, 2> n_tiles);

  template <class Rng>
  GridGraph(std::array<Label, 2> grid_length, std::array<Label, 2> n_tiles, double prob_connection,
            Rng& rng, bool construct = true);

  template <class Rng>
  GridGraph(Label grid_length, Label n_tiles, double prob_connection, Rng& rng);

  const std::vector<Edge>& get_edges() const {
    return edges_;
  }

  // TODO: remove
  std::vector<Edge>& get_edges() {
    return edges_;
  }

  Label get_nodes() const {
    return nodes_;
  }

  template <class Concurrency>
  void broadcast(const Concurrency& concurrency);

protected:
  Label coordinatesToId(Label x, Label y) const;
  template <class Rng>
  void buildGraph(Label x_min, Label x_max, Label y_min, Label y_max, Rng& rng, double prob);

  const std::array<Label, 2> grid_size_;
  const std::array<Label, 2> tile_size_;
  const std::array<Label, 2> n_tiles_per_dim_;

  std::vector<Label> tile_starts_;

  std::vector<Edge> edges_;
  Label nodes_;
};

GridGraph::GridGraph(std::array<Label, 2> grid_size, std::array<Label, 2> n_tiles)
    : grid_size_(grid_size),
      n_tiles_per_dim_(n_tiles),
      tile_size_{util::ceilDiv(grid_size[0], n_tiles[0]), util::ceilDiv(grid_size[1], n_tiles[1])},
      nodes_(grid_size_[0] * grid_size_[1]) {}

template <class Rng>
GridGraph::GridGraph(Label grid_length, Label n_tiles, double prob_connection, Rng& rng)
    : GridGraph(std::array<Label, 2>{grid_length, grid_length},
                std::array<Label, 2>{n_tiles, n_tiles}, prob_connection, rng) {}

template <class Rng>
GridGraph::GridGraph(std::array<Label, 2> grid_size, std::array<Label, 2> n_tiles,
                     double prob_connection, Rng& rng, bool construct)
    : grid_size_(grid_size),
      n_tiles_per_dim_(n_tiles),
      tile_size_{util::ceilDiv(grid_size[0], n_tiles[0]), util::ceilDiv(grid_size[1], n_tiles[1])},
      nodes_(grid_size_[0] * grid_size_[1]) {
  if (grid_size[0] >= (std::numeric_limits<Label>::max() - 1) / grid_size[1])
    throw(std::out_of_range("Use a bigger representation."));

  tile_starts_.resize(n_tiles_per_dim_[0] * n_tiles_per_dim_[1], 0);
  for (int tx = 0; tx < n_tiles_per_dim_[0]; ++tx)
    for (int ty = 0; ty < n_tiles_per_dim_[0]; ++ty) {
      const int index = tx + ty * n_tiles_per_dim_[0];

      const Label this_tile_x_size = tx == n_tiles_per_dim_[0] - 1
                                         ? grid_size_[0] - (n_tiles_per_dim_[0] - 1) * tile_size_[0]
                                         : tile_size_[0];
      const Label this_tile_y_size = ty == n_tiles_per_dim_[1] - 1
                                         ? grid_size_[1] - (n_tiles_per_dim_[1] - 1) * tile_size_[1]
                                         : tile_size_[1];
      if (index + 1 < tile_starts_.size()) {
        tile_starts_[index + 1] = tile_starts_[index] + this_tile_x_size * this_tile_y_size;
        assert(tile_starts_[index + 1] < nodes_);
      }
    }

  if (construct)
    buildGraph(0, grid_size_[0], 0, grid_size_[1], rng, prob_connection);
}

template <class Rng>
void GridGraph::buildGraph(Label x_min, Label x_max, Label y_min, Label y_max, Rng& rng,
                           double prob_connection) {
  auto generate_connection = [&](Label x1, Label y1, Label x2, Label y2) {
    if (rng() < prob_connection) {  // Insert an edge with probability prob connection.
      edges_.push_back(Edge(coordinatesToId(x1, y1), coordinatesToId(x2, y2)));
    }
  };

  for (Label y = y_min; y < y_max; ++y)
    for (Label x = x_min; x < x_max; ++x) {
      if (x + 1 < grid_size_[0])
        generate_connection(x, y, x + 1, y);
      if (y + 1 < grid_size_[1])
        generate_connection(x, y, x, y + 1);
    }
}

inline Label GridGraph::coordinatesToId(Label x, Label y) const {
  const Label tile_x = x / tile_size_[0];
  const Label tile_y = y / tile_size_[1];

  const Label tile_start = tile_starts_[tile_x + n_tiles_per_dim_[0] * tile_y];

  // Coordinates relative to the tile start.
  x -= tile_x * tile_size_[0];
  y -= tile_y * tile_size_[1];

  const Label id =  tile_start + x + y * tile_size_[0];
  assert(id < nodes_);
  return id;
}

template <class Concurrency>
void GridGraph::broadcast(const Concurrency& concurrency) {
  concurrency.broadcast(nodes_);
  concurrency.broadcast(edges_);
}

}  // graph
