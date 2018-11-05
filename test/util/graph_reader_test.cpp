#include "util/graph_reader.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <cstring> 
#include <iostream>
const string filename = "graph_7_4.adjlist"; 


TEST(GraphReaderDeathTest, FileShouldNotExist) {
    util::GraphReader g; 
    EXPECT_EXIT(g.read_graph_from_adjacency_list("gsasdfg.adjlist"),::testing::ExitedWithCode(1), ""); 
}

/*
TEST(GraphReaderTest, GivesResult) {
    graph::GraphReader g; 
    std::cout << "JUHUUUUUU" << std::endl; 
    vector results = g.read_graph_from_adjacency_list(filename); 
    EXPECT_TRUE(results.size() > 0); 
}*/