#pragma once

#include <thread>

#include <boost/fiber/future.hpp>
#include <boost/fiber/algo/round_robin.hpp>
#include <boost/fiber/algo/work_stealing.hpp>

namespace parallel {

class ThreadPool {
public:
  using Scheduler = boost::fibers::algo::work_stealing;
  //  using Scheduler = boost::fibers::algo::round_robin;

  // Creates a pool with the specified number of threads.
  ThreadPool(size_t n_threads = 0);

  ThreadPool(const ThreadPool& other) = delete;
  ThreadPool(ThreadPool&& other) = default;

  // Adds to the queue of tasks the execution of f(args...). This method is thread safe.
  // Returns: a future to the return value of f(args...).
  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
      -> boost::fibers::future<typename std::result_of<F(Args...)>::type>;

  // The destructor concludes all the pending work gracefully before merging all spawned threads.
  ~ThreadPool();

  // Returns the number of threads used by this class.
  std::size_t size() const {
    return workers_.size();
  }

  // Returns a global instance.
  static ThreadPool& get_instance() {
    static ThreadPool global_pool(1);
    return global_pool;
  }

private:
  void workerLoop(int n_threads);

  std::vector<std::thread> workers_;
};

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> boost::fibers::future<typename std::result_of<F(Args...)>::type> {
  return boost::fibers::async(boost::fibers::launch::post, std::forward<F>(f),
                              std::forward<Args>(args)...);
}

}  // parallel
