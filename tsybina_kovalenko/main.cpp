#include <fstream>
#include <iostream>
#include <string>

#include "graph.hpp"
#include "graph_generator.hpp"
#include "printing.hpp"

void write_to_file(const std::string& data, const std::string& file_path) {
  std::ofstream out(file_path, std::ios::out | std::ios::trunc);
  out << data << "\n";
  out.close();
}

bool is_number(std::string str) {
  for (auto it = str.begin(); it != str.end(); ++it) {
    if (*it == '-')
      continue;
    if (isdigit(*it) == false) {
      return false;
    }
  }
  return true;
}

Graph::Depth handle_depth_input() {
  Graph::Depth depth;
  bool is_depth_correct = false;
  do {
    std::cout << "Enter graph depth: " << std::flush;
    std::string depth_string;
    std::cin >> depth_string;
    try {
      depth = stoi(depth_string);
      if (!is_number(depth_string))
        throw std::invalid_argument("");
      is_depth_correct = depth >= 0;
      if (!is_depth_correct) {
        std::cout << "Alas, depth can't be negative :(" << std::endl;
      }
    } catch (const std::invalid_argument&) {
      std::cout << "Alas, depth should be a number :(" << std::endl;
    }
  } while (!is_depth_correct);
  return depth;
}

int handle_new_vertices_count_input() {
  int new_vertices_count;
  bool is_new_vertices_count_correct = false;
  do {
    std::cout << "Enter new vertices count for each vertex: " << std::flush;
    std::string new_vertices_count_string;
    std::cin >> new_vertices_count_string;
    try {
      new_vertices_count = stoi(new_vertices_count_string);
      if (!is_number(new_vertices_count_string))
        throw std::invalid_argument("");
      is_new_vertices_count_correct = new_vertices_count >= 0;
      if (!is_new_vertices_count_correct) {
        std::cout << "Alas, new vertices count can't be negative :("
                  << std::endl;
      }
    } catch (const std::invalid_argument&) {
      std::cout << "Alas, new vertices count should be a number :("
                << std::endl;
    }
  } while (!is_new_vertices_count_correct);
  return new_vertices_count;
}

int main() {
  auto depth = handle_depth_input();
  auto new_vertices_count = handle_new_vertices_count_input();

  const auto graph = GraphGenerator({depth, new_vertices_count}).generate();
  const auto graph_json = printing::json::print_graph(graph);
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");

  return 0;
}
