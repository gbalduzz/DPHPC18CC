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

  Label tx = round(tx_f);
  int ty = p / tx;

  // Ensure tx and ty are divisiors of p.
  if (tx * ty != p) {
    auto divisor = [](Label a, Label b) -> bool { return a / b * b == a; };

    const int first_guess = tx > tx_f ? -1 : 1;

    for (int delta = 1; delta < p / 2; ++delta)
      for (int sign : std::array<int, 2>{first_guess, -first_guess})
        if (divisor(p, (tx + sign * delta))) {
          tx = tx + sign * delta;
          ty = p / tx;
          goto loop_exit;
        }
  }
loop_exit:

  assert(tx >= 1 && ty >= 1 && tx * ty == p);
  return std::array<Label, 2>{tx, ty};
}

}  // util
