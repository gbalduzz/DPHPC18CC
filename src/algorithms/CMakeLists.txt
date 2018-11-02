
add_library(algorithms serial_connected_components.cpp parallel_connected_components.cpp)
target_link_libraries(algorithms PUBLIC ${BOOST_LIBS};util)
