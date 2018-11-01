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

  Label parent(Label i) const{
   return parent_[i];
  }

  Label representative(Label i) const;

  bool isRoot(Label i) const;

  void compress();

private:
  std::vector<Label> parent_;
};

}  // graph

