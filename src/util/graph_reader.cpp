#include "util/graph_reader.hpp"
#include "io/json11.hpp"
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;

namespace util {
vector<graph::Edge> GraphReader::read_graph_from_adjacency_list(string filename) {
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
    boost::split(nr, content, [](char c) { return c == ' '; });

    for (int i = 1; i < nr.size(); i++) {
      edge_list.push_back(graph::Edge(std::stoi(nr[0]), std::stoi(nr[i])));
    }
  }

  adjacency_list.close();

  return edge_list;
};

/**
 * Decided against this...
 * Keep it for now as I might reconsider :)
 * */
vector<vector<graph::Label>> GraphReader::read_connected_components_from_json(string filename) {
  /** TODO TALK ABOUT ACTUALLY USEFUL JSON PARSER!!*/

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(filename, pt);
  // vector<ConnectedComponent
  for (auto it : pt) {
    for (auto i : it.second) {
      std::cout << i.second.data() << endl;
    }
  }
  vector<graph::Edge> edge_list;
  vector<vector<graph::Label>> edges;
  return edges;
}

/**
 * if the first char in one line is an a, then the line describes an edge
 * Otherwise: Comment or similar
 *
 * */
std::vector<graph::Edge> GraphReader::read_graph_from_DIMACS_challenge(string filename) {
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
      edges = stoi(nr[6])/2; // only half of the edges get read
      outfile << nodes << " " << edges << endl;
    }

	  if (content.front() != 'a') {
        continue;
    }
    
	 
    // if the first char in one line is an a, then the line describes an edge

    vector<string> nr;
    boost::split(nr, content, [](char c) { return c == ' '; });
    //Hack. DIMAC files have the first node at 1, the min communication expects first number to be 0. 
    outfile << (stoi(nr[1]) -1)<< " " << (stoi(nr[2]) -1) << " " << nr[3] << endl;
    getline(file, content); // only half of the edges get read
  
  }

  file.close();
  outfile.close();

}


int GraphReader::vertexNumber(const std::vector<graph::Edge>& edges) {
  graph::Label max_id = 0;
  for (const auto& edge : edges) {
    max_id = std::max(max_id, std::max(edge.first, edge.second));
  }
  return max_id + 1;
}

}  // namespace util
