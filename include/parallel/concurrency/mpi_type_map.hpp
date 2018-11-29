// This struct maps C++ types to MPI types.

#pragma once

#include <complex>
#include <cstdlib>
#include <type_traits>
#include <mpi.h>

namespace parallel {

template <typename T>
struct MPITypeMap {
  template <typename = std::enable_if_t<std::is_enum<T>::value>>
  static MPI_Datatype value() {
    return MPITypeMap<std::underlying_type_t<T>>::value();
  }
};

template <>
struct MPITypeMap<bool> {
public:
  static MPI_Datatype value() {
    return MPI_CXX_BOOL;
  }
};

template <>
struct MPITypeMap<char> {
public:
  static MPI_Datatype value() {
    return MPI_CHAR;
  }
};

template <>
struct MPITypeMap<int> {
public:
  static MPI_Datatype value() {
    return MPI_INT;
  }
};

template <>
struct MPITypeMap<unsigned int> {
public:
  static MPI_Datatype value() {
    return MPI_UNSIGNED;
  }
};

template <>
struct MPITypeMap<std::size_t> {
public:
  static MPI_Datatype value() {
    return MPI_UNSIGNED_LONG;
  }
};

}  // parallel
