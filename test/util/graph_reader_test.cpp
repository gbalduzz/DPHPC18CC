#include "util/graph_reader.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <cstring> 

const string filename = "graph_7_4.adjlist"; 


TEST(GraphReaderDeathTest, FileShouldNotExist) {
    graph::GraphReader g; 
    EXPECT_EXIT(g.read_graph_from_adjacency_list("gsasdfg.adjlist"),::testing::ExitedWithCode(1), ""); 
}

TEST(GraphReaderTest, GivesResult) {
    graph::GraphReader g; 
    vector results = g.read_graph_from_adjacency_list(filename); 
    EXPECT_TRUE(results.size() > 0); 
}
