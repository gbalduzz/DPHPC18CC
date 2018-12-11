// This file partitions a rectangle into p tiles.

#pragma once

#include <array>
#include <cmath>
#include <cassert>

#include "graph/edge.hpp"

namespace util {

inline std::array<graph::Label, 2> partition(graph::Label nx, graph::Label ny, int p) {
  using graph::Label;

  auto round = [](double x) {
    if (x - Label(x) >= 0.5)
      return Label(x) + 1;
    else
      return Label(x);
  };

  const double tx_f = std::sqrt(double(nx) * p / double(ny));
  const double ty_f = double(p) / tx_f;

  Label tx, ty;

  if (std::abs(tx_f - Label(tx_f)) > std::abs(ty_f - Label(ty_f))) {
    tx = round(tx_f);
    ty = tx > tx_f ? std::floor(ty_f) : std::ceil(ty_f);
  }
  else {
    ty = round(ty_f);
    tx = ty > ty_f ? std::floor(tx_f) : std::ceil(tx_f);
  }

  while (tx * ty > p) {
    if (tx > ty) {
      ++tx;
      --ty;
    }
    else {
      --tx;
      ++ty;
    }
  }

  assert(tx >= 1 && ty >= 1);
  return std::array<Label, 2>{tx, ty};
}

}  // util
