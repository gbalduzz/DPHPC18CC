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

  int tx, ty;

  if (std::abs(tx_f - int(tx_f)) > std::abs(ty_f - int(ty_f))) {
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
  return std::array<int, 2>{tx, ty};
}

}  // util
