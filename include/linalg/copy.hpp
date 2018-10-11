// This file provides memory copy utilities.

#pragma once

#include <cassert>
#include <complex>
#include <cstring>

namespace linalg {
namespace util {
// dca::linalg::util::

template <typename ScalarType>
inline void memoryCopy(ScalarType *dest, const ScalarType *src, size_t sz) {
  std::memcpy(dest, src, sz * sizeof(ScalarType));
}

template <typename ScalarType>
void memoryCopy(ScalarType *dest, int ld_dest, const ScalarType *src,
                int ld_src, std::pair<int, int> size) {
  assert(size.first <= ld_dest);
  assert(size.first <= ld_src);
  assert(size.first >= 0);
  assert(size.second >= 0);

  size_t ncols = size.second;
  for (size_t i = 0; i < ncols; ++i) {
    memoryCopy(dest + i * ld_dest, src + i * ld_src, size.first);
  }
}

} // util
} // linalg
