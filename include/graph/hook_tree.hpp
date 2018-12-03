#pragma once

#include <atomic>
#include <iostream>
#include <cassert>
#include <numeric>
#include <vector>
#include <string>
#include <algorithm>

#include "graph/edge.hpp"

namespace graph {

class HookTree {
public:
  HookTree(Label n, bool parallel = false);

  // constructor to initialize hooktree from parent array
  HookTree(std::vector<Label>&& labels);

  HookTree& operator+=(const HookTree& rhs);

  // Hooks i to j. No check is performed on the inputs.
  void hook(Label i, Label j);

  // Find the representatives of i and j, and hook the maximum representative to the minimum in a
  // thread safe manner.
  void hookToMinSafe(Label i, Label j);

  // As above, but skip the connection of nodes i and j to their representative.
  void hookAndUpdate(Label repr_i, Label repr_j, Label i, Label j);

  Label representative(Label i) const;
  inline void setRepresentative(Label i, Label repr) {
    parent_[i] = repr;
  }

  void compress();

  void compress(const Label i) {
    parent_[i] = representative(i);
  }

  Label parent(Label i) const {
    return parent_[i];
  }

  bool isRoot(Label i) const {
    return i == parent_[i];
  }

  std::string toString() const;

  const std::vector<Label>& getParents() const {
    return parent_;
  }

  const unsigned getNumConnectedComponents() const;

private:
  // Same as hook, but it is atomic and allowed to fail.
  bool hookAtomic(Label i, Label j);

  std::vector<Label> parent_;
};

inline HookTree::HookTree(const Label n, const bool parallel) : parent_(n) {
  if (!parallel) {
    std::iota(parent_.begin(), parent_.end(), 0);
  }
  else {
#pragma omp parallel for
    for (int i = 0; i < n; ++i) {
      parent_[i] = i;
    }
  }
}

inline HookTree::HookTree(std::vector<Label>&& labels) {
  parent_ = std::move(labels);
}

inline HookTree& HookTree::operator+=(const HookTree& rhs) {
  assert(parent_.size() == rhs.parent_.size());
#pragma omp parallel for schedule(static)
  for (int i = 0; i < parent_.size(); ++i) {
    // If the representation in the two three (repr1 and repr2) is different hook the two
    // representatives.
    const Label repr1 = parent_[i];
    const Label repr2 = rhs.parent_[i];

    if (repr1 != repr2) {
      hookToMinSafe(repr1, repr2);
    }
  }
  return *this;
}

inline void HookTree::hook(Label i, Label j) {
  parent_[i] = j;
}

inline void HookTree::hookAndUpdate(Label repr_i, Label repr_j, Label i, Label j) {
  assert(isRoot(repr_i) && isRoot(repr_j));
  parent_[repr_i] = repr_j;
  parent_[i] = repr_j;
  parent_[j] = repr_j;
}

inline bool HookTree::hookAtomic(Label i, Label j) {
  return std::atomic_compare_exchange_weak(reinterpret_cast<std::atomic<Label>*>(&parent_[i]), &i, j);
}

inline void HookTree::hookToMinSafe(graph::Label i, graph::Label j) {
  bool hooked = false;
  Label repr_i(i), repr_j(j);

  while (!hooked) {
    repr_i = representative(repr_i);
    repr_j = representative(repr_j);

    if (repr_i > repr_j)
      hooked = hookAtomic(repr_i, repr_j);
    else if (repr_i < repr_j)
      hooked = hookAtomic(repr_j, repr_i);
    else
      hooked = true;
  }
}

inline Label HookTree::representative(Label index) const {
  while (index != parent_[index]) {
    index = parent_[index];
  }
  return index;
}

inline void HookTree::compress() {
#pragma omp parallel for schedule(dynamic, 5000)
  for (std::size_t i = 0; i < parent_.size(); ++i) {
    compress(i);
  }
}

inline const unsigned HookTree::getNumConnectedComponents() const {
  const int n = parent_.size();
  std::vector<bool> is_rep(n, false);

  for (int i = 0; i < n; ++i) {
    is_rep[representative(i)] = true;
  }

  unsigned result = 0;
  for (int i = 0; i < n; ++i) {
    result += is_rep[i] ? 1 : 0;
  }

  return result;
}

inline std::string HookTree::toString() const {
  std::string res = "";

  // std::vector<std::vector<Label>> connected_components(parent_.size());
  for (int i = 0; i < parent_.size(); ++i) {
    Label root = representative(parent_[i]);
    res += std::to_string(i) + "->" + std::to_string(root) + "\n";
    // connected_components[root].push_back(i);
  }

  return res;
}

}  // graph
