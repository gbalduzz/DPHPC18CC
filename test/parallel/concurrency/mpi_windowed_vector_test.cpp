#include "parallel/concurrency/mpi_windowed_vector.hpp"

#include <memory>

#include <gtest/gtest.h>

#include "parallel/concurrency/mpi_concurrency.hpp"

std::unique_ptr<parallel::MpiConcurrency> concurrency;

TEST(MPIWindowedVector, All) {
  ASSERT_EQ(2, concurrency->size());

  parallel::MPIWindowedObject<int> v(4);

  for (int i = 0; i < 4; ++i)
    v[i] = concurrency->id();

  v.sync();

  const int other_rank = concurrency->id() == 0 ? 1 :0;
  auto result = v.get(other_rank, 0);

  // TODO: remove second sync.
  v.sync();
  EXPECT_EQ(other_rank, result);
}

int main(int argc, char** argv) {
  concurrency = std::make_unique<parallel::MpiConcurrency>(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  concurrency.release();
  return result;
}
