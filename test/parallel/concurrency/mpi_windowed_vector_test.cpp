#include "parallel/concurrency/mpi_windowed_vector.hpp"

#include <memory>
#include <future>

#include <gtest/gtest.h>

#include "parallel/concurrency/mpi_concurrency.hpp"
#include "testing/minimalist_printer.hpp"

std::unique_ptr<parallel::MpiConcurrency> concurrency;

TEST(MPIWindowedVector, Get) {
  ASSERT_EQ(2, concurrency->size());

  parallel::MPIWindowedVector<int> v(4);

  for (int i = 0; i < 4; ++i)
    v[i] = concurrency->id();

  v.sync();

  const int other_rank = concurrency->id() == 0 ? 1 : 0;

  EXPECT_EQ(other_rank, v.get(other_rank, 0));
}

TEST(MPIWindowedVector, AtomicCAS) {
  ASSERT_EQ(2, concurrency->size());

  parallel::MPIWindowedVector<int> v(4);

  for (int i = 0; i < 4; ++i)
    v[i] = concurrency->id();

  v.sync();

  if (concurrency->id() == 0) {
    bool result;
    result = v.atomicCAS(1, 2, 1, 5);
    EXPECT_EQ(true, result);
    result = v.atomicCAS(1, 3, -1, 5);
    EXPECT_EQ(false, result);
  }

  v.sync();

  if (concurrency->id() == 1) {
    EXPECT_EQ(5, v[2]);
    EXPECT_EQ(1, v[3]);
  }
}

TEST(MPIWindowedVector, Multithread) {
  parallel::MPIWindowedVector<int> v(2);
  for (int i = 0; i < 2; ++i)
    v[i] = concurrency->id();

  v.sync();

  if (concurrency->id() == 0) {
    std::future<int> f1 = std::async(std::launch::async, [&]() { return v.get(1, 0); });
    std::future<int> f2 = std::async(std::launch::async, [&]() { return v.get(1, 1); });

    EXPECT_EQ(1, f1.get());
    EXPECT_EQ(1, f2.get());
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  concurrency = std::make_unique<parallel::MpiConcurrency>(argc, argv);

  ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
  if (concurrency->id() != 0) {
    delete listeners.Release(listeners.default_result_printer());
    listeners.Append(new ::testing::MinimalistPrinter);
  }

  const auto result = RUN_ALL_TESTS();
  concurrency.release();
  return result;
}
