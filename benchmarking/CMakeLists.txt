add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark ${LIBS})

add_executable(benchmark_mpi benchmark_mpi.cpp)
target_link_libraries(benchmark_mpi ${LIBS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/script_mpi_benchmark.bash ./ COPYONLY)

add_custom_target(USA-road-t.USA.gr
        COMMAND [ -f \"USA-road-t.USA.gr\" ] && echo \"skipping download\" ||
                (wget http://www.dis.uniroma1.it/challenge9/data/USA-road-t/USA-road-t.USA.gr.gz;
                gunzip -k USA-road-t.USA.gr.gz)
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(benchmark USA-road-t.USA.gr)
add_dependencies(benchmark_mpi USA-road-t.USA.gr)
