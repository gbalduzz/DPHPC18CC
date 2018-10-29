#include <iostream>

#include "graph/generate_random_graph.hpp"
#include "io/json11.hpp"

int main(int argc, char** argv) {
  std::string input_name = "input.json";
  if (argc > 1)
    input_name = std::string(argv[1]);

  json11::Json parameters = json11::Json::parseFile(input_name);

  auto edges = graph::generateRandomGraph(parameters["vertices"].int_value(),
                                          parameters["edges"].int_value());

  return 0;
}
