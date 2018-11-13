#pragma once

#include <vector>
#include <cmath>


namespace util {

template <typename T>
T avg(const std::vector<T>& v) {
  T result(0);
  for (auto x : v)
    result += x;
  return result / v.size();
}

template <typename T>
T stddev(const std::vector<T>& v) {
  const T mean = avg(v);
  T result(0);
  for (auto x : v)
    result += (x - mean) * (x - mean);
  return std::sqrt(result / (v.size() - 1.));
}

}  // util
