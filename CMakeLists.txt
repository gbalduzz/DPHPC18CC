cmake_minimum_required(VERSION 3.10)
project(MinCut)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(LIBS src/json11.cpp)

add_executable(min_cut src/main.cpp)
target_link_libraries(min_cut ${LIBS})
