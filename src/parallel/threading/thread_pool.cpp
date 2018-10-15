// This file implements the methods of thread_pool.hpp

#include "parallel/threading/thread_pool.hpp"

namespace parallel {

ThreadPool::ThreadPool(size_t n_threads) {
  // Caller thread registers itself at Scheduler.
  boost::fibers::use_scheduling_algorithm<Scheduler>(n_threads);
  for (int i = 0; i < n_threads - 1; ++i) {
    workers_.emplace_back(&ThreadPool::workerLoop, this, n_threads);
  }
}

ThreadPool::~ThreadPool() {
  for (std::thread& worker : workers_)
    worker.join();
}

void ThreadPool::workerLoop(int thread_count) {
  // Thread registers itself at Scheduler.
  boost::fibers::use_scheduling_algorithm<Scheduler>(thread_count);
}

}  // parallel
