add_executable(graph_scaling graph_scaling.cpp)
target_link_libraries(graph_scaling algorithms graph)

configure_file(plot.py ./ COPYONLY)


