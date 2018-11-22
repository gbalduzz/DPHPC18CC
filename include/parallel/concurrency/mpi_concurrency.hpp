// Wrapper to MPI initialization.
#pragma once

#include <stdexcept>

#include "mpi.h"

namespace parallel {

class MpiConcurrency {
public:
  MpiConcurrency(int argc = 1, char** argv = nullptr) {
    constexpr int required = MPI_THREAD_FUNNELED;
    int provided;
    MPI_Init_thread(&argc, &argv, required, &provided);

    if (required > provided) {
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

private:
  int id_;
  int size_;
};

}  // parallel
