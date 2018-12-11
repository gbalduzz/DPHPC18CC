#pragma once

namespace util {

template <class T, class U>
auto ceilDiv(T a, U b) -> decltype(a + b) {
  static_assert(std::is_integral<T>::value && std::is_integral<U>::value,
                "ceilDiv argument not integer.");
  return (a + b - 1) / b;
}
}
