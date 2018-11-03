add_gtest(serial_connected_components_test LIBS algorithms;graph;util)

configure_file("${PROJECT_SOURCE_DIR}/test/algorithms/labels.txt"
        "${CMAKE_BINARY_DIR}/test/algorithms/labels.txt" COPYONLY)

#WILL HAVE TO FIND A BETTER SOLUTION HERE
configure_file("${PROJECT_SOURCE_DIR}/graphs/graph_0.adjlist"
        "${CMAKE_BINARY_DIR}/test/algorithms/graph_0.adjlist" COPYONLY)

        configure_file("${PROJECT_SOURCE_DIR}/graphs/graph_0_info.txt"
        "${CMAKE_BINARY_DIR}/test/algorithms/graph_0_info.txt" COPYONLY)
