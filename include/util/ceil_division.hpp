#pragma once
#include <type_traits>

namespace util {
// dca::util::

// Preconditions: a >= 0, and b > 0.
template <typename IntegerType>
constexpr typename std::enable_if<std::is_integral<IntegerType>::value, IntegerType>::type ceilDiv(
    IntegerType a, IntegerType b) {
  return (a + b - 1) / b;
}

}  // util

