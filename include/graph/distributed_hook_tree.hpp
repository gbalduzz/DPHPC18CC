#pragma once

#include <atomic>
#include <cassert>
#include <numeric>
#include <vector>
#include <string>
#include <algorithm>

#include <mpi.h>

#include "graph/edge.hpp"

namespace graph {

using RankLabel = Label;

class DistributedHookTree {
public:
  DistributedHookTree(Label n_vertices, RankLabel rank_id, RankLabel n_ranks, unsigned int n_threads);

  Label representativeLocal(Label l) const;

  Label representative(Label l) const;

  // Precondition: i and j are roots
  bool hookAtomicLocal(Label i, Label j);

  bool isRoot(Label i) const;

  bool isLocal(Label i) const;
  RankLabel ownerRank(Label i) const;

  // Hooks the representative with highest index to the lowest one.
  bool hook(Label i, Label j);

  //    // As above, but skip the connection of nodes i and j.
  //    void hookAndUpdate(Label repr_i, Label repr_j, Label i, Label j);
  //
  //    // Same as hook, but it is atomic and allowed to fail.
  //    bool hookAtomic(Label i, Label j);
  //
  //  // As above, but skip the connection of nodes i and j.
  //  bool hookAtomicAndUpdate(Label repr_i, Label repr_j, Label i, Label j);
  //
  //  Label representative(Label i) const;

  void compressLocal();

  const Label* data() const {
    return parent_.data();
  }
  std::size_t size() const {
    return parent_.size();
  }

private:
  const Label n_vertices_;
  const Label vertices_per_rank_;
  const RankLabel n_ranks_;
  const RankLabel rank_;

  const Label range_start_;
  const Label range_end_;

  const unsigned int n_threads_;

  std::vector<Label> parent_;
};

inline DistributedHookTree::DistributedHookTree(graph::Label n_vertices, graph::RankLabel rank_id,
                                                graph::RankLabel n_ranks, unsigned int n_threads)
    : n_vertices_(n_vertices),
      vertices_per_rank_(n_vertices / n_ranks),
      n_ranks_(n_ranks),
      rank_(rank_id),
      range_start_(vertices_per_rank_ * rank_id),
      range_end_(vertices_per_rank_ * (rank_id + 1)),
      n_threads_(n_threads),
      parent_(vertices_per_rank_) {
  assert((n_vertices % n_ranks) == 0);  // TODO support general case.

#pragma omp parallel for num_threads(n_threads_)
  for (int i = 0; i < vertices_per_rank_; ++i)
    parent_[i] = range_start_ + i;
}


inline bool DistributedHookTree::hookAtomicLocal(Label i, Label j) {
  //  assert(isRoot(i) && isRoot(j));
  Label repr_i = parent_[i - range_start_];
  const Label repr_j = parent_[j - range_start_];
  return std::atomic_compare_exchange_weak(
      reinterpret_cast<std::atomic<Label>*>(&parent_[i - range_start_]), &repr_i, repr_j);
}

inline bool DistributedHookTree::isRoot(Label l) const {
  assert(isLocal(l));
  return parent_[l - range_start_] == l;
}

inline bool DistributedHookTree::isLocal(Label l) const {
  return l >= range_start_ && l < range_end_;
}

inline RankLabel DistributedHookTree::ownerRank(graph::Label l) const {
  return l / size();
}

inline Label DistributedHookTree::representativeLocal(Label l) const {
  assert(isLocal(l));
  while (l != parent_[l]) {
    l = parent_[l];
    assert(isLocal(l));
  }
  return l;
}

inline void DistributedHookTree::compressLocal() {
#pragma omp parallel for num_threads(n_threads_) schedule(dynamic, 5000)
  for (std::size_t i = 0; i < parent_.size(); ++i) {
    parent_[i] = representativeLocal(i);
  }
}

inline Label DistributedHookTree::representative(Label label) const {
  Label repr = label;

  do {
    label = repr;

    if (isLocal(label)) {
      repr = parent_[label - range_start_];
    }
    else {
      const RankLabel owner = ownerRank(label);
//      MPI_Get(&repr, 1, MPI_UNSIGNED, owner, label - owner * size(), 1, MPI_UNSIGNED, parent_);
    }
  } while (label != repr);

  return label;
}

inline bool DistributedHookTree::hook(graph::Label i, graph::Label j) {
    throw("");
}

}  // graph
