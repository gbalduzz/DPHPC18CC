add_executable(benchmark_mpi_node_distributed benchmark_mpi_node_distributed.cpp)
target_link_libraries(benchmark_mpi_node_distributed algorithms)

add_executable(benchmark_mpi_distributed_multiinput benchmark_mpi_distributed_multiinput.cpp)
target_link_libraries(benchmark_mpi_distributed_multiinput algorithms)

configure_file(job_daint.pbs.in ./ COPYONLY)
configure_file(submit_weak_scaling.py ./ COPYONLY)
configure_file(plot.py ./ COPYONLY)

