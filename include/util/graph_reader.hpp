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

namespace graph {
class GraphReader {
public:
    std::vector<graph::Edge> read_graph_from_adjacency_list(string filename); 
};
}  // namespace graph

#endif