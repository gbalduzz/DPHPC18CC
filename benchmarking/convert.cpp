#include <iostream>
#include <fstream>

#include "util/graph_reader.hpp"

std::ostream& operator<<(std::ostream& out, std::pair<double, double> p) {
  return out << p.first << "\t" << p.second;
}

int main(int argc, char** argv) {
  const std::string filename = argc > 1 ? std::string(argv[1]) : "USA-road-t.NY.gr";
  const std::string output_filename = argc > 2 ? std::string(argv[2]) : "out.hdf5";

  std::vector<graph::Edge> edges;
  graph::Label n;
  util::GraphReader::readCommAvoidingInput(filename, edges, n);
  std::cout << "Loadeded graph.\n";




  return 0;
}
