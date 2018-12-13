#pragma once

#include <vector>
#include <cmath>

namespace util {

template <typename T>
double avg(const std::vector<T>& v) {
  double result(0);
  for (auto x : v)
    result += x;
  return result / v.size();
}

template <typename T>
double stddev(const std::vector<T>& v) {
  const double mean = avg(v);
  T result(0);
  for (auto x : v)
    result += (x - mean) * (x - mean);
  return std::sqrt(result / (v.size() - 1.));
}

}  // util
