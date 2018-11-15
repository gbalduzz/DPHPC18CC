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

  std::string toString() {

    std::string res = "";

    //std::vector<std::vector<Label>> connected_components(parent_.size());
    for(int i=0; i<parent_.size(); ++i) {
        Label root = representative(parent_[i]);
        res += std::to_string(i) + "->" + std::to_string(root) + "\n";
        //connected_components[root].push_back(i);
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
