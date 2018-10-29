#pragma once

#include <vector>

// TODO: delete this file

namespace graph {
namespace mock {

class HookTree {
public:
  HookTree(int n);

  // Hook the three containing i to the three containing j.
  void hook(int i, int j);

  int representative(int i) const{
   return representatives_[i];
  }

  bool isStar(int i) const;

  bool isSingleton(int i) const;

  void compress();

private:
  std::vector<int> representatives_;
};

}  // mock
}  // graph

