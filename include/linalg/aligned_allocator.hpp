#pragma once

#include <vector>

namespace linalg {
namespace util {
// linalg::util::


template <typename T>
class AlignedAllocator : public std::allocator<T> {
public:
  T* allocate(std::size_t n) {
    T* ptr;
    int err = posix_memalign((void**)&ptr, 128, n * sizeof(T));
    if (err)
      throw(std::bad_alloc());
    return ptr;
  }

  void deallocate(T*& ptr, std::size_t /*n*/ = 0) noexcept {
    free(ptr);
    ptr = nullptr;
  }
};

}  // util
}  // linalg
