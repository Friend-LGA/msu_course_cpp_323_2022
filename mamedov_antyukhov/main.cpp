#include <fstream>
#include <iostream>
#include <string>
#include "graph.hpp"
#include "graph_generator.hpp"
#include "printing.hpp"

int handle_depth_input() {
  std::string input;
  int output, tries = 0;
  constexpr int max_tries_num = 15;
  std::cout << "\nEnter the estimated depth of the graph (depth is a "
               "non-negative integer):"
            << std::endl;
  while (tries < max_tries_num && std::cin >> input) {
    ++tries;
    if (sscanf(input.c_str(), "%d", &output)) {
      if (output >= 0)
        return output;
    }
    if (!(tries % 5))
      std::cout << "Read the description carefully!";
    else
      std::cout << "Incorrect depth!";
    std::cout << " Try one more time." << std::endl;
  }
  throw std::runtime_error("Depth has not been passed!");
  return output;
}

int handle_new_vertices_count_input() {
  std::string input;
  int output, tries = 0;
  constexpr int max_tries_num = 15;
  std::cout
      << "\nEnter \"new_vertices_count\" variable (new_vertices_count is a "
         "non-negative integer):"
      << std::endl;
  while (tries < max_tries_num && std::cin >> input) {
    ++tries;
    if (sscanf(input.c_str(), "%d", &output)) {
      if (output >= 0)
        return output;
    }
    if (!(tries % 5))
      std::cout << "Read the description carefully!";
    else
      std::cout << "Incorrect new_vertices_count variable!";
    std::cout << " Try one more time." << std::endl;
  }

  throw std::runtime_error("New_vertices_count has been passed!");
  return output;
}

void write_to_file(const std::string& graph_str, const std::string& file_name) {
  std::ofstream fout(file_name);
  fout << graph_str;
}

int main() {
  const int depth = handle_depth_input();
  const int new_vertices_count = handle_new_vertices_count_input();
  auto params = GraphGenerator::Params(depth, new_vertices_count);
  const auto generator = GraphGenerator(std::move(params));
  const auto graph = generator.generate();

  const auto graph_json = printing::json::print_graph(graph);
  write_to_file(graph_json, "graph.json");
  std::cout << "Graph representation:" << std::endl;
  std::cout << graph_json << std::endl;
  return 0;
}
