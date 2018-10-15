add_library(parallel threading/thread_pool.cpp)

target_link_libraries(parallel PUBLIC boost_system;boost_fiber;boost_thread;boost_context;pthread)
