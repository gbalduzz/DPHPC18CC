// This file implements the methods of thead_pool.hpp

#include "parallel/threading/thread_pool.hpp"

namespace parallel {

ThreadPool::ThreadPool(size_t n_threads) : stop_(false), active_id_(0) {
  enlarge(n_threads);
}

ThreadPool::~ThreadPool() {
  stop_ = true;
  for (auto& condition : condition_)
    condition->notify_one();
  for (std::thread& worker : workers_)
    worker.join();
}

void ThreadPool::enlarge(size_t n_threads) {
  if (workers_.size() >= n_threads)
    return;

  // Lock all queues to avoid modifications while enlarging the pool.
  std::vector<std::unique_lock<std::mutex>> locks;
  for (auto& mutex_ptr : queue_mutex_)
    locks.emplace_back(std::unique_lock<std::mutex>(*mutex_ptr));

  // Create enough resources for the new workers.
  queue_mutex_.reserve(n_threads);
  condition_.reserve(n_threads);
  tasks_.reserve(n_threads);

  // Release the locks on the existing queues.
  locks.clear();

  for (size_t id = workers_.size(); id < n_threads; ++id) {
    queue_mutex_.emplace_back(std::make_unique<std::mutex>());
    condition_.emplace_back(std::make_unique<std::condition_variable>());
    tasks_.emplace_back(std::make_unique<std::queue<std::packaged_task<void()>>>());

    // Start a loop on each new thread.
    workers_.emplace_back(&ThreadPool::workerLoop, this, id);
  }
}

void ThreadPool::workerLoop(int id) {
  while (true) {
    std::packaged_task<void()> task;

    {  // Acquire new task.
      std::unique_lock<std::mutex> lock(*queue_mutex_[id]);
      condition_[id]->wait(lock, [this, id] { return stop_ || !tasks_[id]->empty(); });
      // If all the work is done and no more will be posted, return.
      if (stop_ && tasks_[id]->empty())
        return;
      task = std::move(tasks_[id]->front());
      tasks_[id]->pop();
    }
    // Execute task.
    task();
  }
}

}  // parallel
