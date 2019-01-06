// Wrapper to MPI initialization.
#pragma once

#include <stdexcept>

#include "mpi.h"

#include "mpi_type_map.hpp"

namespace parallel {

class MpiConcurrency {
public:
  MpiConcurrency(int argc = 1, char** argv = nullptr, int required_flag = MPI_THREAD_FUNNELED) {
    int provided;
    MPI_Init_thread(&argc, &argv, required_flag, &provided);

    if (required_flag > provided) {
      throw(std::runtime_error("MPI initialization failed."));
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size_);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_);
  }

  ~MpiConcurrency() {
    MPI_Finalize();
  }

  int id() const {
    return id_;
  }
  int size() const {
    return size_;
  }

  template <class T>
  void broadcast(T& obj, int root = 0);

private:
  int id_;
  int size_;
};

template <class T>
void MpiConcurrency::broadcast(T& obj, int root) {
  MPI_Bcast(&obj, 1, MPITypeMap<T>::value(), root, MPI_COMM_WORLD);
}

}  // parallel
