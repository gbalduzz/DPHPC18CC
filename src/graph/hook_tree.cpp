#include "graph/hook_tree.hpp"

#include <numeric>

namespace graph {

HookTree::HookTree(Label n) : parent_(n) {
  std::iota(parent_.begin(), parent_.end(), 0);
}

bool HookTree::hook(Label i, Label j) {
  parent_[representative(i)] = representative(j);
  return true;
}

Label HookTree::representative(Label index) const {
  while (index != parent_[index]) {
    index = parent_[index];
  }
  return index;
}

void HookTree::compress() {
  for (std::size_t i = 0; i < parent_.size(); ++i) {
    parent_[i] = representative(i);
  }
}

}  // graph
