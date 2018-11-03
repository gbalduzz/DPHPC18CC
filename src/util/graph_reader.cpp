#include "util/graph_reader.hpp"

using namespace std;

namespace graph {
    vector<graph::Edge> GraphReader::read_graph_from_adjacency_list(string filename) {
        int sum = 0;
        int x;
        ifstream adjacency_list;

        adjacency_list.open(filename);
        if (!adjacency_list) {
            exit(1);  // terminate with error
        }

        vector<graph::Edge> edge_list; 

        while (!adjacency_list.eof()) {
            string content; 
            getline(adjacency_list, content);
            vector<string> nr; 
            boost::split(nr, content, [](char c){return c == ' ';}); 

            for (int i = 1; i < content.size(); i++) { 
                edge_list.push_back(graph::Edge(content[0], content[i])); 
            }
        }

        adjacency_list.close();
        return edge_list; 
     
    }; 
};
// namespace graph
