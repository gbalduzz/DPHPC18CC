#include "graph/mock_hook_tree.hpp"

#include <numeric>

// TODO: delete this file

namespace graph {
namespace mock {

HookTree::HookTree(Label n) : representatives_(n) {
  std::iota(representatives_.begin(), representatives_.end(), 0);
}

void HookTree::hook(Label i, Label j) {
  representatives_[representatives_[i]] = representatives_[j];
}

bool HookTree::isStar(Label i) const {
  const Label repr = representatives_[i];
  return repr == representatives_[repr];
}

void HookTree::compress() {
  for (std::size_t i = 0; i < representatives_.size(); ++i) {
    if (!isStar(i)) {
      Label repr = representatives_[i];
      while (repr != representatives_[repr])
        repr = representatives_[repr];
      representatives_[i] = repr;
    }
  }
}

}  // mock
}  // graph
