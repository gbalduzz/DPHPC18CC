#ifndef GRAPH_READER_H

#define GRAPH_READER_H

#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "graph/edge.hpp"
#include <vector>

using namespace std;

namespace util {
class GraphReader {
public:
  /**
   * Reads in an adjacency list and returns a list containting all edges specified in the adjacency
   * list.
   * */
  std::vector<graph::Edge> read_graph_from_adjacency_list(string filename);

  /**
   * Reads in a json file and returns a list with node names for each connected component */
  std::vector<std::vector<graph::Label>> read_connected_components_from_json(string filename);

  /**
   *  Read Edges from this: http://www.dis.uniroma1.it/challenge9/download.shtml challenge
   *  */
  std::vector<graph::Edge> read_graph_from_DIMACS_challenge(string filename);

  /**
* Returns the number ov vertices in the graph
* */
  int vertexNumber(const std::vector<graph::Edge>& edges);

  void read_graph_from_DIMACS_challenge_to_file(string filename);

  std::vector<graph::Edge> read_graph_from_paramat(string filename);

  static void readCommAvoidingInput(const std::string& filename, std::vector<graph::Edge>& edges,
                                    graph::Label& nodes);
};
}  // namespace graph

#endif
