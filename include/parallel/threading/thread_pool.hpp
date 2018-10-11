// This file implements a thread pool based on the file
// https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h.
// See COPYING for the license of the original work.

#pragma once

#include <condition_variable>
#include <iostream>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace parallel {

class ThreadPool {
public:
  // Creates a pool with the specified number of threads.
  ThreadPool(size_t n_threads = 0);

  ThreadPool(const ThreadPool& other) = delete;
  ThreadPool(ThreadPool&& other) = default;

  // Enlarges the pool to the specified number of threads if it is larger than the current number of
  // threads.
  void enlarge(std::size_t n_threads);

  // Adds to the queue of tasks the execution of f(args...). This method is thread safe.
  // Returns: a future to the return value of f(args...).
  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

  // The destructor concludes all the pending work gracefully before merging all spawned threads.
  ~ThreadPool();

  // Returns the number of threads used by this class.
  std::size_t size() const {
    return workers_.size();
  }

  // Returns a global instance.
  static ThreadPool& get_instance() {
    static ThreadPool global_pool;
    return global_pool;
  }

private:
  void workerLoop(int id);

  std::vector<std::thread> workers_;
  std::vector<std::unique_ptr<std::queue<std::packaged_task<void()>>>> tasks_;

  std::vector<std::unique_ptr<std::mutex>> queue_mutex_;
  std::vector<std::unique_ptr<std::condition_variable>> condition_;
  std::atomic<bool> stop_;
  std::atomic<unsigned int> active_id_;
};

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;
  unsigned int id = active_id_++;
  id = id % size();

  // Enqueue a new task so that the function f with arguments args will be executed by the worker
  // with index id.
  auto task =
      std::packaged_task<return_type()>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task.get_future();
  {
    std::unique_lock<std::mutex> lock(*queue_mutex_[id]);

    if (stop_)
      throw std::runtime_error("enqueue on stopped ThreadPool");

    tasks_[id]->emplace(std::move(task));
  }
  condition_[id]->notify_one();

  return res;
}

}  // parallel
