// Wrapper to MPI initialization.
#pragma once

#include <stdexcept>
#include <vector>
#include <type_traits>

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
  void broadcast(T& obj, int root = 0) const;

  template <class T>
  void broadcast(std::vector<T>& v, int root = 0) const;

private:
  int id_;
  int size_;
};

template <class T>
void MpiConcurrency::broadcast(T& obj, int root) const {
  MPI_Bcast(&obj, sizeof(T), MPI_CHAR, root, MPI_COMM_WORLD);
}

template <class T>
void MpiConcurrency::broadcast(std::vector<T>& v, int root) const {
  std::size_t n = v.size();
  broadcast(n, root);
  v.resize(n);

  constexpr std::size_t block_size = std::numeric_limits<int>::max();

  for (std::size_t start = 0; start < n; start += block_size) {
    std::size_t msg_size = start + block_size <= n ? block_size : n - start;
    MPI_Bcast(v.data() + start, msg_size, MPITypeMap<T>::value(), root, MPI_COMM_WORLD);
  }
}

}  // parallel
