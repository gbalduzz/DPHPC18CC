add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark ${LIBS})

add_executable(benchmark_mpi benchmark_mpi.cpp)
target_link_libraries(benchmark_mpi ${LIBS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/script_mpi_benchmark.bash ./ COPY_ONLY)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(
        USA_road_archieve
        PREFIX "USA_road_archieve"
        URL http://www.dis.uniroma1.it/challenge9/data/USA-road-t/USA-road-t.USA.gr.gz
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        DOWNLOAD_NO_EXTRACT 1
)

add_custom_target(USA-road-t.USA.gr
        COMMAND [ -f \"USA-road-t.USA.gr\" ] && echo \"skipping $file\" || gunzip -k USA_road_archieve/src/USA-road-t.USA.gr.gz; mv USA_road_archieve/src/USA-road-t.USA.gr ./
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS USA_road_archieve)

add_dependencies(benchmark USA-road-t.USA.gr)
add_dependencies(benchmark_mpi USA-road-t.USA.gr)
