#ifndef GRAPH_READER_H

#define GRAPH_READER_H

#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "graph/edge.hpp"
#include <vector>


using namespace std;

namespace util {
class GraphReader {
public:
    /**
     * Reads in an adjacency list and returns a list containting all edges specified in the adjacency list. 
     * */
    std::vector<graph::Edge> read_graph_from_adjacency_list(string filename); 

    /**
     * Reads in a json file and returns a list with node names for each connected component */ 
    std::vector<std::vector<graph::Label>> read_connected_components_from_json(string filename); 

};
}  // namespace graph

#endif