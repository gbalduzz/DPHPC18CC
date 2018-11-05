add_gtest(serial_connected_components_test LIBS algorithms)
add_gtest(parallel_connected_components_test LIBS algorithms)

configure_file("${PROJECT_SOURCE_DIR}/test/algorithms/labels.txt"
        "${CMAKE_BINARY_DIR}/test/algorithms/labels.txt" COPYONLY)
