#pragma once

#include <atomic>
#include <cassert>
#include <numeric>
#include <vector>

#include "graph/edge.hpp"

namespace graph {

class HookTree {
public:
  HookTree(Label n, bool parallel = false);

  // Hooks i to j.
  // Precondition: i and j are roots.
  void hook(Label i, Label j);

  // As above, but skip the connection of nodes i and j.
  void hookAndUpdate(Label repr_i, Label repr_j, Label i, Label j);

  // Same as hook, but it is atomic and allowed to fail.
  bool hookAtomic(Label i, Label j);

  // As above, but skip the connection of nodes i and j.
  bool hookAtomicAndUpdate(Label repr_i, Label repr_j, Label i, Label j);

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

private:
  std::vector<Label> parent_;
};

inline HookTree::HookTree(const Label n, const bool parallel) : parent_(n) {
  if (!parallel) {
    std::iota(parent_.begin(), parent_.end(), 0);
  }
  else {
#pragma omp parallel for
    for (int i = 0; i < n; ++i)
      parent_[i] = i;
  }
}

inline void HookTree::hook(Label i, Label j) {
  assert(isRoot(i) && isRoot(j));
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

inline bool HookTree::hookAtomicAndUpdate(Label repr_i, Label repr_j, Label i, Label j) {
  const bool result = std::atomic_compare_exchange_weak(
      reinterpret_cast<std::atomic<Label>*>(&parent_[repr_i]), &repr_i, repr_j);
  if (parent_[i] != repr_i) {
    parent_[i] = repr_i;
  }
  if (parent_[j] != repr_j) {
    parent_[j] = repr_j;
  }
  return result;
}

inline Label HookTree::representative(Label index) const {
  while (index != parent_[index]) {
    index = parent_[index];
  }
  return index;
}

inline void HookTree::compress() {
  for (std::size_t i = 0; i < parent_.size(); ++i) {
    compress(i);
  }
}

}  // graph
