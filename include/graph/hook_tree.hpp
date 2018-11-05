#pragma once

#include <atomic>
#include <cassert>
#include <numeric>
#include <vector>

#include "graph/edge.hpp"

namespace graph {

class HookTree {
public:
  HookTree(Label n);

  // Hooks i to j.
  // Precondition: i and j are roots.
  void hook(Label i, Label j);

  // Same as above, but it is atomic and allowed to fail.
  bool hookAtomic(Label i, Label j);

  Label representative(Label i) const;

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

inline HookTree::HookTree(Label n) : parent_(n) {
  std::iota(parent_.begin(), parent_.end(), 0);
}

inline void HookTree::hook(Label i, Label j) {
  assert(isRoot(i) && isRoot(j));
  parent_[i] = parent_[j];
}

inline bool HookTree::hookAtomic(Label i, Label j) {
  return std::atomic_compare_exchange_weak(reinterpret_cast<std::atomic<Label>*>(&parent_[i]), &i, j);
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
