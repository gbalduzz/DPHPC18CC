#include <iostream>
#include <fstream>

#include "algorithms/parallel_connected_components.hpp"
#include "graph/generate_random_graph.hpp"
#include "io/json11.hpp"
#include "util/timer.hpp"

int main(int argc, char** argv) {
  std::string input_name = "input.json";
  if (argc > 1)
    input_name = std::string(argv[1]);

  json11::Json parameters = json11::Json::parseFile(input_name);

  const int n = parameters["vertices"].int_value();
  const int m = parameters["edges"].int_value();
  auto edges = graph::generateRandomGraph(n, m);
  std::cout << "Generated random graph.\n";

  const int n_threads = parameters["threads"].int_value();

  auto start = util::getTime();
  auto result = algorithms::parallelConnectedComponents(n, edges, n_threads);
  auto end = util::getTime();

  std::cout << "Serial contraction took " << util::getDiff(start, end) << " s" << std::endl;

  std::ofstream out("labels.txt");
  for (int i = 0; i < n; ++i)
    out << i << ":\t" << result.representative(i) << "\n";
  out.close();

  return 0;
}
