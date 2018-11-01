#pragma once

#include "typedefs.hpp"
#include <vector>
// TODO: implement

namespace graph {

    class HookTree {
    public:
      using VertexIdx = typename Edge::first_type;

      HookTree(int n) {};

      // Hook the three containing i to the three containing j.
      virtual void hook(VertexIdx i, VertexIdx j) = 0;

      virtual VertexIdx representative(VertexIdx i) const = 0;

      virtual bool isStar(VertexIdx i) const = 0;

      virtual bool isSingleton(VertexIdx i) const = 0;

      virtual void makeStar(VertexIdx i) = 0;

    };

}  // graph
