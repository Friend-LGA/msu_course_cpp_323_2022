#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "config.hpp"
#include "graph.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "graph_printing.hpp"
#include "logger.hpp"

namespace {
using Graph = uni_course_cpp::Graph;
using GraphGenerator = uni_course_cpp::GraphGenerator;
using Logger = uni_course_cpp::Logger;
}  // namespace

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

int handle_graphs_count_input() {
  int graphs_count;
  bool is_graphs_count_correct = false;
  do {
    std::cout << "Enter graphs count: " << std::flush;
    std::string graphs_count_string;
    std::cin >> graphs_count_string;
    try {
      graphs_count = stoi(graphs_count_string);
      if (!is_number(graphs_count_string))
        throw std::invalid_argument("");
      is_graphs_count_correct = graphs_count >= 0;
      if (!is_graphs_count_correct) {
        std::cout << "Alas, graphs count can't be negative :(" << std::endl;
      }
    } catch (const std::invalid_argument&) {
      std::cout << "Alas, graphs count should be a number :(" << std::endl;
    }
  } while (!is_graphs_count_correct);
  return graphs_count;
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

void prepare_temp_directory() {
  if (!std::filesystem::exists(uni_course_cpp::config::kTempDirectoryPath))
    std::filesystem::create_directory(
        uni_course_cpp::config::kTempDirectoryPath);
}

std::string generation_started_string(int number_of_graph) {
  std::stringstream generation_started_string;
  generation_started_string << "Graph " << number_of_graph
                            << ", Generation Started";
  return generation_started_string.str();
}

std::string generation_finished_string(int number_of_graph,
                                       std::string graph_description) {
  std::stringstream generation_finished_string;
  generation_finished_string << "Graph " << number_of_graph
                             << ", Generation Finished " << graph_description;
  return generation_finished_string.str();
}

int main() {
  auto depth = handle_depth_input();
  auto new_vertices_count = handle_new_vertices_count_input();
  auto graphs_count = handle_graphs_count_input();

  prepare_temp_directory();

  const auto generator = GraphGenerator({depth, new_vertices_count});
  auto& logger = Logger::get_logger();

  for (int i = 0; i < graphs_count; ++i) {
    logger.log(generation_started_string(i));
    const auto graph = generator.generate();
    const auto graph_description = uni_course_cpp::printing::print_graph(graph);
    logger.log(generation_finished_string(i, graph_description));
    const auto graph_json = uni_course_cpp::printing::json::print_graph(graph);
    write_to_file(graph_json,
                  (std::string)(uni_course_cpp::config::kTempDirectoryPath) +
                      "graph_" + std::to_string(i) + ".json");
  }

  return 0;
}
