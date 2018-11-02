#include "util/get_bounds.hpp"
#include <cassert>

namespace util {
// util::

std::pair<int, int> getBounds(int id, int num_threads, const std::pair<int, int>& current_bounds) {
  assert(id >= 0 && id < num_threads);
  assert(current_bounds.second >= current_bounds.first && current_bounds.first >= 0);

  const int size = current_bounds.second - current_bounds.first;
  const int fair_division = size / num_threads;
  const int remainder = size % num_threads;

  if (id < remainder) {
    const int workload = fair_division + 1;
    return std::make_pair(current_bounds.first + workload * id,
                          current_bounds.first + workload * (id + 1));
  }
  else {
    const int start =
        current_bounds.first + (fair_division + 1) * remainder + fair_division * (id - remainder);
    return std::make_pair(start, start + fair_division);
  }
}

}  // util
