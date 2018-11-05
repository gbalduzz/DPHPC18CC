#pragma once

#include <thread>

#include <boost/thread/barrier.hpp>
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

  ~ThreadPool();

  // Returns the number of threads used by this class.
  std::size_t size() const {
    return workers_.size() + 1;
  }

  // Returns a global instance.
  static ThreadPool& get_instance(int n) {
    static ThreadPool global_pool(n);
    if (n != global_pool.size())
      throw(std::logic_error("Change of the pool size is not supported."));
    return global_pool;
  }

private:
  void workerLoop(int n_threads);

  std::vector<std::thread> workers_;
  boost::fibers::mutex mutex_;
  boost::fibers::condition_variable_any done_variable_;
  boost::barrier scheduler_registered_;
  bool done_ = true;
};

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> boost::fibers::future<typename std::result_of<F(Args...)>::type> {
  return boost::fibers::async(boost::fibers::launch::post, std::forward<F>(f),
                              std::forward<Args>(args)...);
}

}  // parallel
