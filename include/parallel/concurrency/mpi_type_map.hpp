// This class maps C++ types to MPI types.

#pragma once

#include <complex>
#include <cstdlib>
#include <type_traits>
#include <mpi.h>

namespace parallel {

template <typename T>
class MPITypeMap {
public:
  static constexpr std::size_t factor() {
    return 1;
  }

  template <typename = std::enable_if_t<std::is_enum<T>::value>>
  static MPI_Datatype value() {
    return MPITypeMap<std::underlying_type_t<T>>::value();
  }
};

template <>
class MPITypeMap<bool> {
public:
  static std::size_t factor() {
    return 1;
  }

  static MPI_Datatype value() {
    return MPI_CXX_BOOL;
  }
};

template <>
class MPITypeMap<char> {
public:
  static std::size_t factor() {
    return 1;
  }

  static MPI_Datatype value() {
    return MPI_CHAR;
  }
};

template <>
class MPITypeMap<int> {
public:
  static std::size_t factor() {
    return 1;
  }

  static MPI_Datatype value() {
    return MPI_INT;
  }
};

template <>
class MPITypeMap<std::size_t> {
public:
  static std::size_t factor() {
    return 1;
  }

  static MPI_Datatype value() {
    return MPI_UNSIGNED_LONG;
  }
};

template <>
class MPITypeMap<float> {
public:
  static std::size_t factor() {
    return 1;
  }

  static MPI_Datatype value() {
    return MPI_FLOAT;
  }
};

template <>
class MPITypeMap<double> {
public:
  static std::size_t factor() {
    return 1;
  }

  static MPI_Datatype value() {
    return MPI_DOUBLE;
  }
};

template <>
class MPITypeMap<std::complex<float>> {
public:
  static std::size_t factor() {
    return 2;
  }

  static MPI_Datatype value() {
    return MPI_FLOAT;
  }
};

template <>
class MPITypeMap<std::complex<double>> {
public:
  static std::size_t factor() {
    return 2;
  }

  static MPI_Datatype value() {
    return MPI_DOUBLE;
  }
};

}  // parallel
