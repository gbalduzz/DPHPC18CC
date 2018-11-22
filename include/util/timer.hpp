#pragma once

#include <chrono>
#include <iostream>

namespace util {

using Time = typeof(std::chrono::high_resolution_clock::now());

inline Time getTime() {
  return std::chrono::high_resolution_clock::now();
}

// Prints in seconds.
inline double getDiff(Time t0, Time tf) {
  return std::chrono::duration<double>(tf - t0).count();
}

}  // util
