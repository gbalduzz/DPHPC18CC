#pragma once

#include "typedefs.hpp"
// TODO: implement

namespace graph {

class HookTree {
public:
  using VertexIdx = typename Edge::first_type;

  HookTree(VertexIdx n);

  // Hook the three containing i to the three containing j.
  void hook(VertexIdx i, VertexIdx j);

  VertexIdx representative(VertexIdx i) const;

  bool isStar(VertexIdx i) const;

  bool isSingleton(VertexIdx i) const;

  //  void makeStar(VertexIdx i);

  // Returns true if any changes has been made during compression.
  bool compress();
};

}  // graph
