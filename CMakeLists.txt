cmake_minimum_required(VERSION 3.9)
project(MinCut)

set(CMAKE_CXX_STANDARD 17)

#Threading options
SET(CMAKE_CXX_FLAGS "-pthread -fopenmp")

# System libraries.
set(BOOST_DIR "/usr/local/boost" CACHE STRING "Path to boost.")
include_directories(${BOOST_DIR}/include)
link_directories(${BOOST_DIR}/lib)
set(BOOST_LIBS boost_system;boost_context;boost_fiber)


# Local libraries.
include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(src)
set(LIBS algorithms;io;graph;parallel;util)

# Main application
add_executable(main src/main.cpp)
target_link_libraries(main ${LIBS})
configure_file("${PROJECT_SOURCE_DIR}/src/input.json"
               "${CMAKE_BINARY_DIR}/input.json" COPYONLY)

# Testing.

include(cmake/testing.cmake)
enable_testing()

add_subdirectory(test)
