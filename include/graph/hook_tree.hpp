#pragma once

// TODO: implement

namespace graph {

class HookTree {
public:
  HookTree(int n);

  // Hook the three containing i to the three containing j.
  void hook(int i, int j);

  int representative(int i) const;

  int isStar(int i) const;

  int isSingleton(int i) const;

  void makeStar(int i);

};

}  // graph
