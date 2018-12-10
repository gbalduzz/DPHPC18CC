// Wrapper to MPI initialization.
#pragma once

#include <stdexcept>

#include "fompi_wrapper.hpp"

namespace parallel {

class MpiConcurrency {
public:
  MpiConcurrency(int argc = 1, char** argv = nullptr, int required_flag = MPI_THREAD_FUNNELED) {
    int provided;
    MMPI_INIT_THREAD(&argc, &argv, required_flag, &provided);

    if (required_flag > provided) {
      throw(std::runtime_error("MPI initialization failed."));
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size_);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_);
  }

  ~MpiConcurrency() {
    MMPI_FINALIZE();
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
