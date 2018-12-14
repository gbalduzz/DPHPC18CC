#pragma once

#include <atomic>
#include <cassert>
#include <numeric>
#include <vector>
#include <string>
#include <algorithm>

#include <mpi.h>

#include "graph/edge.hpp"
#include "parallel/concurrency/mpi_windowed_vector.hpp"

namespace graph {

using RankLabel = Label;

class DistributedHookTree {
public:
  DistributedHookTree(Label vertices_per_rank, RankLabel rank_id, RankLabel n_ranks,
                      unsigned int n_threads);

  Label representativeLocal(Label l) const;

  Label representative(Label l) const;

  bool isRoot(Label i) const;

  bool isLocal(Label i) const;
  RankLabel ownerRank(Label i) const;

  // Hooks the representative with highest index to the lowest one.
  void hookToMinSafe(Label i, Label j);

  void hookToMinSafeLocal(Label i, Label j);

  void compressLocal();
  void compress();

  const Label* data() const {
    return parent_.data();
  }
  std::size_t size() const {
    return parent_.size();
  }

  void sync() const {
    parent_.sync();
  }

private:
  bool hookAtomicLocal(Label i, Label j);
  Label partialRepresentative(Label i) const;

  const Label vertices_per_rank_;
  const RankLabel n_ranks_;
  const RankLabel rank_;

  const Label range_start_;
  const Label range_end_;

  const unsigned int n_threads_;

  parallel::MPIWindowedVector<Label> parent_;
};

inline DistributedHookTree::DistributedHookTree(graph::Label vertices_per_rank,
                                                graph::RankLabel rank_id, graph::RankLabel n_ranks,
                                                unsigned int n_threads)
    : vertices_per_rank_(vertices_per_rank),
      n_ranks_(n_ranks),
      rank_(rank_id),
      range_start_(vertices_per_rank_ * rank_id),
      range_end_(vertices_per_rank_ * (rank_id + 1)),
      n_threads_(n_threads),
      parent_(vertices_per_rank_) {
#pragma omp parallel for num_threads(n_threads_)
  for (Label i = 0; i < vertices_per_rank_; ++i) {
    parent_[i] = range_start_ + i;
  }
}

// Hook i to j.
inline bool DistributedHookTree::hookAtomicLocal(Label repr_i, Label repr_j) {
  assert(isLocal(repr_i));
  return std::atomic_compare_exchange_weak(
      reinterpret_cast<std::atomic<Label>*>(&parent_[repr_i - range_start_]), &repr_i, repr_j);
}

inline void DistributedHookTree::hookToMinSafeLocal(Label i, Label j) {
  bool hooked = false;
  Label repr_i(i), repr_j(j);

  while (!hooked) {
    repr_i = representativeLocal(repr_i);
    repr_j = representativeLocal(repr_j);

    if (repr_i > repr_j)
      hooked = hookAtomicLocal(repr_i, repr_j);
    else if (repr_i < repr_j)
      hooked = hookAtomicLocal(repr_j, repr_i);
    else
      hooked = true;
  }

  if (i != repr_i)
    parent_[i - range_start_] = repr_i;
  if (j != repr_j)
    parent_[j - range_start_] = repr_j;
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
  while (l != parent_[l - range_start_]) {
    l = parent_[l - range_start_];
    assert(isLocal(l));
  }
  return l;
}

inline Label DistributedHookTree::partialRepresentative(Label l) const {
  while (isLocal(l) && l != parent_[l - range_start_]) {
    l = parent_[l - range_start_];
  }
  return l;
}

inline void DistributedHookTree::compressLocal() {
#pragma omp parallel for num_threads(n_threads_) schedule(dynamic, 5000)
  for (Label i = 0; i < parent_.size(); ++i) {
    parent_[i] = representativeLocal(i + range_start_);
  }
}

inline void DistributedHookTree::compress() {
#pragma omp parallel for num_threads(n_threads_) schedule(dynamic, 5000)
  for (std::size_t i = 0; i < parent_.size(); ++i) {
    parent_[i] = representative(i + range_start_);
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
      repr = parent_.globalGet(label);
    }
  } while (label != repr);

  return label;
}

inline void DistributedHookTree::hookToMinSafe(const Label i, const Label j) {
  bool hooked = false;
  Label repr_i(i), repr_j(j);

  while (!hooked) {
    repr_i = partialRepresentative(repr_i);
    repr_j = partialRepresentative(repr_j);

    if (repr_i == repr_j)
      break;

    Label& from = repr_i > repr_j ? repr_i : repr_j;
    const Label& to = repr_i > repr_j ? repr_j : repr_i;

    if (isLocal(from)) {  // Hook local to remote.
      hooked = hookAtomicLocal(from, to);
    }
    else {  // Hook remote to local.
      Label result = parent_.atomicCAS(from, from, to);
      if (result == from)
        hooked = true;
      else
        from = result;
    }
  }

  auto skip_connection = [&](Label vertex, Label repr) {
    if (vertex != repr) {
      if (isLocal(vertex))
        parent_[vertex - range_start_] = repr;
//      else
//        parent_.immediateGlobalWrite(vertex, repr);
    }
  };

  skip_connection(i, repr_i);
  skip_connection(j, repr_j);
}

}  // graph
