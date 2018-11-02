#include "algorithms/parallel_connected_components.hpp"

#include <omp.h>

#include "parallel/threading/thread_pool.hpp"
#include "util/get_bounds.hpp"
#include "util/ceil_division.hpp"

namespace algorithms {

graph::HookTree parallelConnectedComponents(const unsigned n, std::vector<graph::Edge>& edges,
                                            const int n_threads, const int n_tasks) {
  const unsigned int m = edges.size();
  //  if (n_threads > 0) {
  //    omp_set_num_threads(n_threads);
  //  }
  graph::HookTree tree(n);

  parallel::ThreadPool pool(n_threads);

  while (true) {
    bool changes = false;

    {
      std::vector<boost::fibers::future<void>> tasks;
      for (int task_id = 0; task_id < n_tasks; ++task_id) {
        tasks.emplace_back(pool.enqueue([&, task_id]() {

          auto bounds = util::getBounds(task_id, n_tasks, std::make_pair(0, m));

          for (unsigned int e_id = bounds.first; e_id < bounds.second; ++e_id) {
            auto& edge = edges[e_id];
            if (!edge.isValid())
              continue;

            const auto repr_i = tree.representative(edge.first);
            const auto repr_j = tree.representative(edge.second);

            if (repr_i != repr_j) {
              changes = true;
              const bool hooked = tree.hookRoots(std::max(repr_i, repr_j), std::min(repr_i, repr_j));

              if (hooked) {
                edge.markInvalid();
              }
            }
          }
        }));
      }

      for (auto& task : tasks)
        task.get();
    }

    std::vector<boost::fibers::future<void>> tasks;
    for (int task_id = 0; task_id < n_tasks; ++task_id) {
      tasks.emplace_back(pool.enqueue([&, task_id]() {
        auto bounds = util::getBounds(task_id, n_tasks, std::make_pair(0, n));
        for (unsigned int i = bounds.first; i < bounds.second; ++i) {
          tree.compress(i);
        }
      }));
    }

    for (auto& task : tasks)
      task.get();

    if (!changes) {
      break;
    }
  }

  return tree;
}

}  // algorithms
