add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark ${LIBS})

add_executable(benchmark_mpi benchmark_mpi.cpp)
target_link_libraries(benchmark_mpi ${LIBS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/script_mpi_benchmark.bash ./ COPYONLY)

