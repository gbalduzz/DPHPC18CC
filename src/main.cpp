#include <iostream>

#include "algorithms/serial_connected_components.hpp"
#include "graph/generate_random_graph.hpp"
#include "io/json11.hpp"

int main(int argc, char** argv) {
  std::string input_name = "input.json";
  if (argc > 1)
    input_name = std::string(argv[1]);

  json11::Json parameters = json11::Json::parseFile(input_name);

  const int n = parameters["vertices"].int_value();
  const int m = parameters["edges"].int_value();
  auto edges = graph::generateRandomGraph(n, m);

  auto result = algorithms::serialConnectedComponents(n, edges);

  for (int i = 0; i < n; ++i)
    std::cout << i << ":\t" << result.representative(i) << "\n";

  std::cout << std::endl;
  return 0;
}
