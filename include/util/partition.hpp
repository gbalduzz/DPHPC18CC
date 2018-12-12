// This file partitions a rectangle into p tiles.

#pragma once

#include <array>
#include <cmath>
#include <cassert>

namespace util {

inline std::array<int, 2> partition(int nx, int ny, int p) {
  auto round = [](float x) {
    if (x - int(x) >= 0.5)
      return int(x) + 1;
    else
      return int(x);
  };

  const float tx_f = std::sqrt(float(nx) * p / float(ny));
  const float ty_f = float(p) / tx_f;

  int tx = round(tx_f);
  int ty = p / tx;

  // Ensure tx and ty are divisiors of p.
  if (tx * ty != p) {
    auto divisor = [](int a, int b) -> bool { return a / b * b == a; };

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
  return std::array<int, 2>{tx, ty};
}

}  // util
