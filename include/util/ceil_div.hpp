#pragma once

namespace util {

template <class T>
T ceilDiv(T a, T b) {
  static_assert(std::is_integral<T>::value, "ceilDiv argument not integer.");
  return (a + b - 1) / b;
}

}
