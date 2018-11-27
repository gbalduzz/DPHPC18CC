#pragma once

#include <atomic>
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

  std::string toString() {
    std::string res = "";

    // std::vector<std::vector<Label>> connected_components(parent_.size());
    for (int i = 0; i < parent_.size(); ++i) {
      Label root = representative(parent_[i]);
      res += std::to_string(i) + "->" + std::to_string(root) + "\n";
      // connected_components[root].push_back(i);
    }

    /*
    for(int i=0; i<connected_components.size(); ++i) {
        if(connected_components[i].size() > 0) {
            for(int j=0; j<connected_components[i].size(); ++j) {
                res += std::to_string(connected_components[i][j]) + " | ";
            }
            res += "\n";
        }
    }
    */

    return res;
  }

  const std::vector<Label>& getParents() const {
    return parent_;
  }

  const unsigned getNumConnectedComponents() const;

private:
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
// loop over all nodes in other tree
#pragma omp parallel for schedule(static)
  for (int i = 0; i < rhs.parent_.size(); ++i) {
    // there is nothing we need to do if i is already the root
    if (rhs.isRoot(i)) {
      continue;
    }

    Label other_representative = rhs.representative(i);
    Label this_other_representative_representative = this->representative(other_representative);
    Label this_representative = this->representative(i);

    // edge already exists in this tree => there is nothing to do
    if (this_representative == this_other_representative_representative) {
      continue;
    }

    this->hook(std::max(this_representative, this_other_representative_representative),
               std::min(this_representative, this_other_representative_representative));
  }
  return *this;
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
  parent_[i] = result ? repr_j : repr_i;
  parent_[j] = repr_j;
  return result;
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

}  // graph
