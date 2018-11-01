#pragma once

#include <vector>

#include "graph/edge.hpp"

// TODO: delete this file

namespace graph {
namespace mock {

class HookTree {
public:
  HookTree(Label n);

  // Hook the three containing i to the three containing j.
  void hook(Label i, Label j);

  Label representative(Label i) const{
   return representatives_[i];
  }

  bool isStar(Label i) const;

  bool isSingleton(Label i) const;

  void compress();

private:
  std::vector<Label> representatives_;
};

}  // mock
}  // graph

