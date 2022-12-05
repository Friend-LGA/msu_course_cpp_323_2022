#include <fstream>
#include <iostream>
#include <string>
#include "graph.hpp"
#include "printing.hpp"

constexpr int kVerticesCount = 14;

Graph generate_graph() {
  auto graph = Graph();

  for (int i = 0; i < kVerticesCount; ++i) {
    graph.add_vertex();
  }

  graph.add_edge(0, 1);
  graph.add_edge(0, 2);
  graph.add_edge(0, 3);
  graph.add_edge(1, 4);
  graph.add_edge(1, 5);
  graph.add_edge(1, 6);
  graph.add_edge(2, 7);
  graph.add_edge(2, 8);
  graph.add_edge(3, 9);
  graph.add_edge(4, 10);
  graph.add_edge(5, 10);
  graph.add_edge(6, 10);
  graph.add_edge(7, 11);
  graph.add_edge(8, 11);
  graph.add_edge(9, 12);
  graph.add_edge(10, 13);
  graph.add_edge(11, 13);
  graph.add_edge(12, 13);

  return graph;
}

void write_to_file(const std::string& output_string,
                   const std::string& file_name) {
  std::ofstream output(file_name);
  if (output.is_open())
    output << output_string;
  else
    std::cout << "File is not accessible!\n";
}

int main() {
  const auto graph = generate_graph();
  const auto graph_json = printing::json::print_graph(graph);
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");
  return 0;
}
