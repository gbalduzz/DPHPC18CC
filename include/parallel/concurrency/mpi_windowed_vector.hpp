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
class MPIWindowedObject {
public:
  MPIWindowedObject(std::size_t size);

  ~MPIWindowedObject();

  T get(unsigned int rank, std::size_t idx) const;

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
MPIWindowedObject<T>::MPIWindowedObject(const std::size_t size) : size_(size) {
  MPI_Comm_size(MPI_COMM_WORLD, &n_ranks_);

  checkMPI(MPI_Alloc_mem(sizeof(T) * size, MPI_INFO_NULL, &data_));

  // TODO: set the appropriate MPI_INFO.
  checkMPI(MPI_Win_create(data_, size, sizeof(T), MPI_INFO_NULL, MPI_COMM_WORLD, &window_));
}

template <class T>
MPIWindowedObject<T>::~MPIWindowedObject() {
  MPI_Win_free(&window_);
  MPI_Free_mem((void*)data_);
}

template <class T>
T MPIWindowedObject<T>::get(const unsigned int target_rank, const std::size_t idx) const {
  assert(idx < size_);

  T result;
  checkMPI(MPI_Get(&result, 1, MPITypeMap<T>::value(), target_rank, idx, 1, MPITypeMap<T>::value(),
                   window_));
  return result;
}

//
// template <class T>
// void MPIWindowedObject<T>::atomicPut(const int target_rank) const {
//  MPI_Accumulate(data_, 1, MPITypeMap<T>::value(), target_rank, 0, 1, MPITypeMap<T>::value(),
//                 MPI_REPLACE, window_);
//}

template <class T>
void MPIWindowedObject<T>::sync() const {
  // TODO: set the appropriate assert.
  MPI_Win_fence(0, window_);
}

}  // parallel
