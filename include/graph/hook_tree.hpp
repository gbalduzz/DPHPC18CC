#pragma once

#include <vector>

#include "graph/edge.hpp"

namespace graph {

class HookTree {
public:
  HookTree(Label n);

  // Tries to hook the tree containing i to the three containing j.
  // Returns: true if success.
  bool hook(Label i, Label j);

  // Same as above, but i and j are guaranteed to be root vertices by the caller.
  bool hookRoots(Label i, Label j);

  Label representative(Label i) const;

  void compress();

  Label parent(Label i) const {
    return parent_[i];
  }

  bool isRoot(Label i) const {
    return i == parent_[i];
  }

private:
  std::vector<Label> parent_;
};

}  // graph
