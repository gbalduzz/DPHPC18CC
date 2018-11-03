add_gtest(graph_reader_test LIBS algorithms;graph;util)

#Test for now... Will have to find a better solution than this... 
configure_file("${PROJECT_SOURCE_DIR}/graphs/graph_7_4.adjlist"
        "${CMAKE_BINARY_DIR}/test/util/graph_7_4.adjlist" COPYONLY)