cmake_minimum_required(VERSION 3.10)
project(MinCut)

set(CMAKE_CXX_STANDARD 17)

# System libraries.
set(BOOST_DIR "/usr/local/boost" CACHE STRING "Path to boost.")
include_directories(${BOOST_DIR}/include)
link_directories(${BOOST_DIR}/lib)

# Local libraries.
include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(src)
set(LIBS io;parallel)

# Main application
add_executable(min_cut src/main.cpp)
target_link_libraries(min_cut ${LIBS})

# Testing.
include(cmake/testing.cmake)
add_subdirectory(test)
