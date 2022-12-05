#include "config.hpp"
#include "graph.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "graph_printing.hpp"
#include "logger.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

using Graph = uni_course_cpp::Graph;
using GraphGenerator = uni_course_cpp::GraphGenerator;
using Logger = uni_course_cpp::Logger;

const Graph::Depth handle_depth_input() {
  std::string input_string;
  Graph::Depth depth_tmp;
  bool is_alpha_flag = false;
  while (true) {
    std::cout << "Enter depth: ";
    std::cin >> input_string;
    std::cout << std::endl;
    for (char next_symbol : input_string) {
      if (!isdigit(next_symbol)) {
        is_alpha_flag = true;
        break;
      }
    }
    if (is_alpha_flag) {
      std::cout << "Incorrect data, depth must be not negative integer.\n";
      is_alpha_flag = false;
      continue;
    }
    depth_tmp = std::atoi(input_string.c_str());
    break;
  }
  return depth_tmp;
}

const int handle_new_vertices_count_input() {
  std::string input_string;
  int new_vertices_count_tmp;
  bool is_alpha_flag = false;
  while (true) {
    std::cout << "Enter new vertices count: ";
    std::cin >> input_string;
    std::cout << std::endl;
    for (char next_symbol : input_string) {
      if (!isdigit(next_symbol)) {
        is_alpha_flag = true;
        break;
      }
    }
    if (is_alpha_flag) {
      std::cout << "Incorrect data, new vertices count must be not negative "
                   "integer.\n";
      is_alpha_flag = false;
      continue;
    }
    new_vertices_count_tmp = std::atoi(input_string.c_str());
    break;
  }
  return new_vertices_count_tmp;
}

const Graph::Depth handle_graphs_count_input() {
  std::string input_string;
  Graph::Depth graphs_count_tmp;
  bool is_alpha_flag = false;
  while (true) {
    std::cout << "Enter graphs count: ";
    std::cin >> input_string;
    std::cout << std::endl;
    for (char next_symbol : input_string) {
      if (!isdigit(next_symbol)) {
        is_alpha_flag = true;
        break;
      }
    }
    if (is_alpha_flag) {
      std::cout
          << "Incorrect data, graphs count must be not negative integer.\n";
      is_alpha_flag = false;
      continue;
    }
    graphs_count_tmp = std::atoi(input_string.c_str());
    break;
  }
  return graphs_count_tmp;
}

void write_to_file(const std::string& content, const std::string& file_name) {
  std::ofstream new_file(uni_course_cpp::config::kTempDirectoryPath +
                         file_name);

  new_file << content;
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

void prepare_temp_directory() {
  if (!std::filesystem::exists(uni_course_cpp::config::kTempDirectoryPath))
    std::filesystem::create_directory(
        uni_course_cpp::config::kTempDirectoryPath);
}

int main() {
  const int depth = handle_depth_input();
  const int new_vertices_count = handle_new_vertices_count_input();
  const int graphs_count = handle_graphs_count_input();
  prepare_temp_directory();

  auto params = GraphGenerator::Params(depth, new_vertices_count);
  const auto generator = GraphGenerator(std::move(params));
  auto& logger = Logger::get_logger();

  for (int number_of_graph = 0; number_of_graph < graphs_count;
       ++number_of_graph) {
    logger.log(generation_started_string(number_of_graph));
    const auto new_graph = generator.generate();
    const auto graph_description =
        uni_course_cpp::printing::print_graph(new_graph);
    logger.log(generation_finished_string(number_of_graph, graph_description));

    const auto graph_json =
        uni_course_cpp::printing::json::print_graph(new_graph);
    write_to_file(graph_json,
                  "graph_" + std::to_string(number_of_graph) + ".json");
  }

  return 0;
}
