#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "linalg/aligned_allocator.hpp"
#include "linalg/copy.hpp"

namespace linalg {
// linalg::

template <typename ScalarType>
class Matrix {
public:
  using ThisType = Matrix<ScalarType>;
  using ValueType = ScalarType;
  using Allocator = util::AlignedAllocator<ScalarType>;

  Matrix(int size = 0);

  Matrix(std::pair<int, int> size);

  // Preconditions: capacity.first >= size.first, capacity.second >=
  // size.second.
  Matrix(std::pair<int, int> size, std::pair<int, int> capacity);

  // Copy and move constructor:
  Matrix(const Matrix<ScalarType>& rhs);
  Matrix(Matrix<ScalarType>&& rhs);

  ~Matrix();

  // Assignment operators:
  Matrix<ScalarType>& operator=(const Matrix<ScalarType>& rhs);
  Matrix<ScalarType>& operator=(Matrix<ScalarType>&& rhs);

  bool operator==(const Matrix<ScalarType>& other) const;
  bool operator!=(const Matrix<ScalarType>& other) const;

  // Returns the (i,j)-th element of the matrix.
  // Preconditions: 0 <= i < size().first, 0 <= j < size().second.
  // This method is available only if device_name == CPU.
  ScalarType& operator()(int i, int j) {
    assert(i >= 0 && i < size_.first);
    assert(j >= 0 && j < size_.second);
    return data_[i + j * leadingDimension()];
  }
  const ScalarType& operator()(int i, int j) const {
    assert(i >= 0 && i < size_.first);
    assert(j >= 0 && j < size_.second);
    return data_[i + j * leadingDimension()];
  }

  // Returns the pointer to the (0,0)-th element.
  ValueType* data() {
    return data_;
  }
  const ValueType* data() const {
    return data_;
  }

  // Returns the pointer to the (i,j)-th element i < size().first and 0 < j <
  // size().second, or
  // a pointer past the end of the range if i == size().first or j ==
  // size().second.
  // Preconditions: 0 <= i <= size().first, 0 <= j <= size().second.
  ValueType* ptr(int i, int j) {
    assert(i >= 0 && i <= size_.first);
    assert(j >= 0 && j <= size_.second);
    return data_ + i + j * leadingDimension();
  }
  const ValueType* ptr(int i, int j) const {
    assert(i >= 0 && i <= size_.first);
    assert(j >= 0 && j <= size_.second);
    return data_ + i + j * leadingDimension();
  }

  bool is_square() const {
    return (size_.first == size_.second);
  }

  const std::pair<int, int> size() const {
    return size_;
  }
  const std::pair<int, int>& capacity() const {
    return capacity_;
  }
  int nrRows() const {
    return size_.first;
  }
  int nrCols() const {
    return size_.second;
  }
  int leadingDimension() const {
    return capacity_.first;
  }

  // Resizes *this to a (new_size * new_size) matrix.
  // Elements added may have any value.
  // Remark: The capacity of the matrix and element pointers do not change
  // if new_size <= capacity().first and new_size <= capacity().second.
  void resize(int new_size) {
    resize(std::make_pair(new_size, new_size));
  }
  // Resizes *this to a (new_size.first * new_size.second) matrix.
  // Elements added may have any value.
  // Remark: The capacity of the matrix and element pointers do not change
  // if new_size.first <= capacity().first and new_size.second <=
  // capacity().second.
  void resize(std::pair<int, int> new_size);

  // Resizes *this to a (new_size * new_size) matrix.
  // The previous elements are not copied, therefore all the elements
  // may have any value after the call to this method.
  // Remark: The capacity of the matrix and element pointers do not change
  // if new_size <= capacity().first and new_size <= capacity().second.
  void resizeNoCopy(int new_size) {
    resizeNoCopy(std::make_pair(new_size, new_size));
  }
  // Resizes *this to a (new_size.first * new_size.second) matrix.
  // The previous elements are not copied, therefore all the elements
  // may have any value after the call to this method.
  // Remark: The capacity of the matrix and element pointers do not change
  // if new_size.first <= capacity().first and new_size.second <=
  // capacity().second.
  void resizeNoCopy(std::pair<int, int> new_size);

  // Releases the memory allocated by *this and sets size and capacity to zero.
  void clear();

  // Prints the values of the matrix elements.
  void print() const;
  // Print memory usage.
  void printFingerprint() const;

private:
  static std::pair<int, int> capacityMultipleOfBlockSize(std::pair<int, int> size);
  inline static size_t nrElements(std::pair<int, int> size) {
    return static_cast<size_t>(size.first) * static_cast<size_t>(size.second);
  }
  // Align to cache line.
  static constexpr int block_size_ = 64 / sizeof(ScalarType);

  std::pair<int, int> size_;
  std::pair<int, int> capacity_;

  ValueType* data_ = nullptr;

  Allocator allocator_;
};

template <typename ScalarType>
Matrix<ScalarType>::Matrix(int size) : Matrix(std::make_pair(size, size)) {}

template <typename ScalarType>
Matrix<ScalarType>::Matrix(std::pair<int, int> size) : Matrix(size, size) {}

template <typename ScalarType>
Matrix<ScalarType>::Matrix(std::pair<int, int> size, std::pair<int, int> capacity)
    : size_(size), capacity_(capacityMultipleOfBlockSize(capacity)) {
  assert(size_.first >= 0 && size_.second >= 0);
  assert(capacity.first >= 0 && capacity.second >= 0);
  assert(capacity.first >= size_.first && capacity.second >= size_.second);
  assert(capacity_.first >= capacity.first && capacity_.second >= capacity.second);

  data_ = allocator_.allocate(nrElements(capacity_));
}

template <typename ScalarType>
Matrix<ScalarType>::Matrix(const Matrix<ScalarType>& rhs) {
  *this = rhs;
}

template <typename ScalarType>
Matrix<ScalarType>::Matrix(Matrix<ScalarType>&& rhs) {
  *this = std::move(rhs);
}

template <typename ScalarType>
Matrix<ScalarType>::~Matrix() {
  allocator_.deallocate(data_);
}

template <typename ScalarType>
void Matrix<ScalarType>::resize(std::pair<int, int> new_size) {
  assert(new_size.first >= 0 && new_size.second >= 0);
  if (new_size.first > capacity_.first || new_size.second > capacity_.second) {
    std::pair<int, int> new_capacity = capacityMultipleOfBlockSize(new_size);

    ValueType* new_data = nullptr;
    new_data = allocator_.allocate(nrElements(new_capacity));
    const std::pair<int, int> copy_size(std::min(new_size.first, size_.first),
                                        std::min(new_size.second, size_.second));
    util::memoryCopy(new_data, new_capacity.first, data_, leadingDimension(), copy_size);
    allocator_.deallocate(data_);

    data_ = new_data;
    capacity_ = new_capacity;
    size_ = new_size;
  }
  else {
    size_ = new_size;
  }
}

template <typename ScalarType>
Matrix<ScalarType>& Matrix<ScalarType>::operator=(const Matrix<ScalarType>& rhs) {
  resizeNoCopy(rhs.size_);
  util::memoryCopy(data_, leadingDimension(), rhs.data_, rhs.leadingDimension(), size_);
  return *this;
}

template <typename ScalarType>
Matrix<ScalarType>& Matrix<ScalarType>::operator=(Matrix<ScalarType>&& rhs) {
  std::swap(data_, rhs.data_);
  std::swap(capacity_, rhs.capacity_);
  std::swap(size_, rhs.size_);

  return *this;
}

template <typename ScalarType>
bool Matrix<ScalarType>::operator==(const Matrix<ScalarType>& other) const {
  if (size() != other.size())
    return nrRows() * nrCols() == 0 and other.nrRows() * other.nrCols() == 0;

  for (int j = 0; j < nrCols(); ++j)
    for (int i = 0; i < nrRows(); ++i)
      if ((*this)(i, j) != other(i, j))
        return false;

  return true;
}

template <typename ScalarType>
bool Matrix<ScalarType>::operator!=(const Matrix<ScalarType>& other) const {
  return not(*this == other);
}

template <typename ScalarType>
void Matrix<ScalarType>::resizeNoCopy(std::pair<int, int> new_size) {
  if (new_size.first > capacity_.first || new_size.second > capacity_.second) {
    size_ = new_size;
    capacity_ = capacityMultipleOfBlockSize(new_size);

    allocator_.deallocate(data_);
    data_ = allocator_.allocate(nrElements(capacity_));
  }
  else {
    size_ = new_size;
  }
}

template <typename ScalarType>
void Matrix<ScalarType>::clear() {
  allocator_.deallocate(data_);
  size_ = capacity_ = std::make_pair(0, 0);
}

template <typename ScalarType>
void Matrix<ScalarType>::print() const {
  printFingerprint();

  std::stringstream ss;
  ss.precision(6);
  ss << std::scientific;

  ss << "\n";
  for (int i = 0; i < nrRows(); ++i) {
    for (int j = 0; j < nrCols(); ++j)
      ss << "\t" << operator()(i, j);
    ss << "\n";
  }

  std::cout << ss.str() << std::endl;
}

template <typename ScalarType>
void Matrix<ScalarType>::printFingerprint() const {
  std::stringstream ss;

  ss << "\n";
  //  ss << "    name: " << name_ << "\n";
  ss << "    size: " << size_.first << ", " << size_.second << "\n";
  ss << "    capacity: " << capacity_.first << ", " << capacity_.second << "\n";
  ss << "    memory-size: " << nrElements(capacity_) * sizeof(ScalarType) * 1.e-6 << "(Mbytes)\n";

  std::cout << ss.str() << std::endl;
}

template <typename ScalarType>
std::pair<int, int> Matrix<ScalarType>::capacityMultipleOfBlockSize(std::pair<int, int> size) {
  assert(size.first >= 0);
  assert(size.second >= 0);

  auto get_new_size = [=](const int size) {
    return size <= 16 ? size : (size + block_size_ - 1) / block_size_ * block_size_;
  };

  size.first = get_new_size(size.first);
  size.second = get_new_size(size.second);

  return size;
}

}  // linalg
