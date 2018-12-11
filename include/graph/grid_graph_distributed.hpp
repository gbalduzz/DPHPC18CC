// This class generates a graph representing a 2D grid with nearest neighbour connections.
// The nodes are Morton ordered.

#pragma once

#include "grid_graph.hpp"

namespace graph {

class GridGraphDistributed final : public GridGraph {
public:
  // Only the local or boundary edges are stored.
  template <class Rng>
  GridGraphDistributed(std::array<Label, 2> grid_length, std::array<Label, 2> n_tiles,
                       double prob_connection, Rng& rng);

  template <class Rng>
  GridGraphDistributed(Label grid_length, Label n_tiles, double prob_connection, Rng& rng);
};

template <class Rng>
GridGraphDistributed::GridGraphDistributed(Label grid_length, Label n_tiles, double prob_connection,
                                           Rng& rng)
    : GridGraphDistributed(std::array<Label, 2>{grid_length, grid_length},
                           std::array<Label, 2>{n_tiles, n_tiles}, prob_connection, rng) {}

template <class Rng>
GridGraphDistributed::GridGraphDistributed(std::array<Label, 2> grid_size,
                                           std::array<Label, 2> n_tiles, double prob_connection,
                                           Rng& rng)
    : GridGraph(grid_size, n_tiles, prob_connection, rng, false) {
  if (grid_size[0] >= (std::numeric_limits<Label>::max() - 1) / grid_size[1])
    throw(std::out_of_range("Use a bigger representation."));

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  const Label tile_y = rank / n_tiles[0];
  const Label tile_x = rank - n_tiles[0] * tile_y;

  const Label size_tile_x = util::ceilDiv(grid_size[0], n_tiles[0]);
  const Label size_tile_y = util::ceilDiv(grid_size[1], n_tiles[1]);

  const Label x0 = tile_x * size_tile_x;
  const Label y0 = tile_y * size_tile_y;

  GridGraph::buildGraph(x0, x0 + size_tile_x, y0, y0 + size_tile_y, rng, prob_connection);
}

}  // graph
