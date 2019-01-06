#pragma once

#include <utility>

namespace graph {

#ifdef WITH_64_BITS
using Label = unsigned long int;
#else
using Label = unsigned int;
#endif

class Edge {
public:
  Edge() = default;
  Edge(const Label i, const Label j) : first(i), second(j) {}

  bool operator==(const Edge& rhs) const {
    return first == rhs.first && second == rhs.second;
  }

  constexpr static Label maxValue() {
    return -1;
  }

  bool isValid() const {
    return first != maxValue();
  }

  void markInvalid() {
    first = maxValue();
  }

public:
  Label first;
  Label second;
};

}  // graph
