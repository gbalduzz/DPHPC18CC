#include "util/graph_reader.hpp"
#include "io/json11.hpp"
#include <sstream>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/foreach.hpp>
#include <iostream>

using namespace std;

namespace util {

// Read graph from adjacency list
// Ignore lines that start with '#', as those are comments.
// If not # or number, will throw error!
// Does not check for duplicates and similar.
/*vector<graph::Edge> GraphReader::read_graph_from_adjacency_list(string filename) {
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

    // Comment
    if (content.front() == '#') {
      continue;
    }
    // If there is no 'space' in a certain line, then the nr array will be empty -> Nodes without
    // connections don't have to be handled seperatly
    boost::split(nr, content, [](char c) { return c == ' '; });

    for (int i = 1; i < nr.size(); i++) {
      edge_list.push_back(graph::Edge(std::stoi(nr[0]), std::stoi(nr[i])));
    }
  }

  adjacency_list.close();

  return edge_list;
};
*/
void GraphReader::readCommAvoidingInput(const std::string& filename,
                                        std::vector<graph::Edge>& edges, graph::Label& nodes) {
  int n_edges, w;

  std::ifstream inp(filename);
  std::string line;
  std::getline(inp, line);
  if (line[0] != '#') {
    throw(std::logic_error("invalid comment."));
  }

  inp >> nodes >> n_edges;
  edges.resize(n_edges);

  for (auto& edge : edges)
    inp >> edge.first >> edge.second >> w;
}

/**
 * if the first char in one line is an a, then the line describes an edge
 * Otherwise: Comment or similar
 *
 * */
/*std::vector<graph::Edge> GraphReader::read_graph_from_DIMACS_challenge(string filename) {
  ifstream file;

  file.open(filename);
  if (!file) {
    exit(1);  // terminate with error
  }

  vector<graph::Edge> edge_list;
  while (!file.eof()) {
    string content;
    getline(file, content);

    // if the first char in one line is an a, then the line describes an edge. Otherwise: Ignore
    if (content.front() != 'a') {
      continue;
    }

    // The first number after the a is the beginning edge, the second the resulting edge, the 3rd
    // the distance. We're ignoring the distance
    vector<string> nr;
    boost::split(nr, content, [](char c) { return c == ' '; });
    edge_list.push_back(graph::Edge(std::stoi(nr[1]), std::stoi(nr[2])));
    assert(nr[1] != "-1" || nr[2] != "-1");
  }

  file.close();

  return edge_list;
}

void GraphReader::read_graph_from_DIMACS_challenge_to_file(string filename) {
  ifstream file;
  ofstream outfile;

  file.open(filename);

  if (!file) {
    exit(1);  // terminate with error
  }
  outfile.open(filename + ".out");
  if (!outfile) {
    exit(1);
  }

  outfile << endl;
  while (!file.eof()) {
    string content;
    getline(file, content);

    // read number of vertices and nodes
    // currently only taking edge in one direction into account
    if (content.front() == 'p') {
      int nodes, edges;
      getline(file, content);
      vector<string> nr;
      boost::split(nr, content, [](char c) { return c == ' '; });
      nodes = stoi(nr[3]);
      edges = stoi(nr[6]) / 2;  // only half of the edges get read
      outfile << nodes << " " << edges << endl;
    }

    if (content.front() != 'a') {
      continue;
    }

    // if the first char in one line is an a, then the line describes an edge

    vector<string> nr;
    boost::split(nr, content, [](char c) { return c == ' '; });
    // Hack. DIMAC files have the first node at 1, the min communication expects first number to be
    // 0.
    outfile << (stoi(nr[1]) - 1) << " " << (stoi(nr[2]) - 1) << " " << nr[3] << endl;
    getline(file, content);  // only half of the edges get read
  }

  file.close();
  outfile.close();
}
*/
int GraphReader::vertexNumber(const std::vector<graph::Edge>& edges) {
  graph::Label max_id = 0;
  for (const auto& edge : edges) {
    max_id = std::max(max_id, std::max(edge.first, edge.second));
  }
  return max_id + 1;
}

}  // namespace util
