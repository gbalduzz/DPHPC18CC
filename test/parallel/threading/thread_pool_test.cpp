// This file tests thread_pool.hpp.

#include "parallel/threading/thread_pool.hpp"

#include <numeric>

#include "gtest/gtest.h"

TEST(ThreadPoolTest, Fibonacci) {
  parallel::ThreadPool pool(std::thread::hardware_concurrency());

  std::function<int(int)> fibonacci = [&](int n) -> int {
    if (n == 1 or n == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      return n;
    }

    auto f1 = pool.enqueue(fibonacci, n - 1);
    auto f2 = pool.enqueue(fibonacci, n - 2);

    return f1.get() + f2.get();
  };

  EXPECT_EQ(21, fibonacci(8));
}

TEST(ThreadPoolTest, FibonacciSerial) {
  std::function<int(int)> fibonacci = [&](int n) -> int {
    if (n == 1 or n == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      return n;
    }

    return fibonacci(n - 1) + fibonacci(n - 2);
  };

  EXPECT_EQ(21, fibonacci(8));
}
