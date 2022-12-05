#include <fstream>
#include <iostream>
#include <stdexcept>
#include "graph.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "graph_printing.hpp"

void write_to_file(const std::string& str, const std::string& filename) {
  std::ofstream out(filename);
  out << str << std::endl;
}

bool is_number(std::string s) {
  for (int i = 0; i < s.length(); i++) {
    if (isdigit(s[i]) == false) {
      return false;
    }
  }

  return true;
}

int handle_depth_input() {
  std::string input;
  do {
    std::cout << "Input depth of graph: ";
    std::cin >> input;
    if (is_number(input) && std::stoi(input) >= 0) {
      return std::stoi(input);
    }

    std::cout << "Incorrect depth" << std::endl;
  } while (true);
}

int handle_new_vertices_count_input() {
  std::string input;
  do {
    std::cout << "Input new_vertices_count: ";
    std::cin >> input;
    if (is_number(input) && std::stoi(input) >= 0) {
      return std::stoi(input);
    }

    std::cout << "Incorrect new_vertices_count" << std::endl;
  } while (true);
}

int main() {
  const int depth = handle_depth_input();
  const int new_vertices_count = handle_new_vertices_count_input();

  auto params = GraphGenerator::Params(depth, new_vertices_count);
  const auto generator = GraphGenerator(std::move(params));
  const auto graph = generator.generate();

  const auto graph_json = printing::json::print_graph(graph);
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");

  return 0;
}
