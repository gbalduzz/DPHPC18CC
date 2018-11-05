cmake_minimum_required(VERSION 3.9)
project(MinCut)

set(CMAKE_CXX_STANDARD 17)

#Threading options
SET(CMAKE_CXX_FLAGS "-pthread -fopenmp")

# System libraries.
set(BOOST_DIR "/usr/local/boost" CACHE STRING "Path to boost.")
#set(BOOST_DIR "C:/Program Files/boost/boost_1_68_0" CACHE STRING "Path to boost.")
include_directories(${BOOST_DIR}/include)
#include_directories(${BOOST_DIR})
link_directories(${BOOST_DIR}/lib)

# Local libraries.
include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(src)
set(LIBS algorithms;io;parallel;util)

# Main application
add_subdirectory(applications)

# Performance test.
add_subdirectory(benchmarking)

# Testing.
enable_testing()
include(cmake/testing.cmake)
add_subdirectory(test)


