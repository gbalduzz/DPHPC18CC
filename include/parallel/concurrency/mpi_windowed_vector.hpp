// This class provides a wrapper to a generic object that can be used for one sided communication

#pragma once

#include <cassert>
#include <iostream>
#include <mpi.h>
#include "parallel/concurrency/mpi_type_map.hpp"

namespace parallel {

void checkMPI(int ret) {
  if (ret != MPI_SUCCESS)
    throw(std::runtime_error("MPI error."));
}

template <class T>
class MPIWindowedVector {
private:
  using Label = unsigned int;

public:
  MPIWindowedVector(std::size_t size);

  ~MPIWindowedVector();

  T get(Label rank, std::size_t idx) const;

  T globalGet(std::size_t idx) const;

  bool atomicCAS(Label rank, Label idx, Label old_val, Label new_val);
  bool atomicCAS(Label global_idx, Label old_val, Label new_val);

  //  void put(int target_rank) const;
  //  void atomicPut(int target_rank) const;
  void sync() const;

  std::size_t size() const {
    return size_;
  }

  T* data() {
    return data_;
  }
  const T* data() const {
    return data_;
  }

  T operator[](std::size_t idx) const {
    assert(idx < size_);
    return data_[idx];
  }

  T& operator[](std::size_t idx) {
    assert(idx < size_);
    return data_[idx];
  }

private:
  int n_ranks_;

  std::size_t size_;
  T* data_ = nullptr;
  MPI_Win window_;
};

template <class T>
MPIWindowedVector<T>::MPIWindowedVector(const std::size_t size) : size_(size) {
  MPI_Comm_size(MPI_COMM_WORLD, &n_ranks_);

  checkMPI(MPI_Alloc_mem(sizeof(T) * size, MPI_INFO_NULL, &data_));

  // TODO: set the appropriate MPI_INFO.
  checkMPI(
      MPI_Win_create(data_, size * sizeof(T), sizeof(T), MPI_INFO_NULL, MPI_COMM_WORLD, &window_));
}

template <class T>
MPIWindowedVector<T>::~MPIWindowedVector() {
  MPI_Win_free(&window_);
  MPI_Free_mem((void*)data_);
}

template <class T>
T MPIWindowedVector<T>::get(const Label target_rank, const std::size_t idx) const {
  assert(idx < size_);

  T result;
  // TODO: check if lock are required on cluster.
  MPI_Win_lock(MPI_LOCK_SHARED, target_rank, 0, window_);
  checkMPI(MPI_Get(&result, 1, MPITypeMap<T>::value(), target_rank, idx, 1, MPITypeMap<T>::value(),
                   window_));
  MPI_Win_unlock(target_rank, window_);

  return result;
}

template <class T>
T MPIWindowedVector<T>::globalGet(const std::size_t idx) const {
  const Label owner = idx / size_;
  return get(owner, idx - size_ * owner);
}

template <class T>
bool MPIWindowedVector<T>::atomicCAS(Label rank, Label idx, Label old_val, Label new_val) {
  assert(idx < size_);
  Label pre_swap_val;

  MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, window_);
      checkMPI(MPI_Compare_and_swap(&new_val, &old_val, &pre_swap_val, MPI_UNSIGNED, rank,
              idx, window_));
      MPI_Win_unlock(rank, window_);

      // TODO: maybe. Use the pre swap value to retry in case of failure.
      return pre_swap_val == old_val;
}

template <class T>
bool MPIWindowedVector<T>::atomicCAS(Label global_idx, Label old_val, Label new_val) {
  const Label rank = global_idx / size_;
  return atomicCAS(rank, global_idx - rank * size_, old_val, new_val);
}

template <class T>
void MPIWindowedVector<T>::sync() const {
  // TODO: set the appropriate assert.
  MPI_Win_fence(0, window_);
}

// template <class T>
// void MPIWindowedVector<T>::atomicPut(const int target_rank) const {
//  MPI_Accumulate(data_, 1, MPITypeMap<T>::value(), target_rank, 0, 1, MPITypeMap<T>::value(),
//                 MPI_REPLACE, window_);
//}

}  // parallel
