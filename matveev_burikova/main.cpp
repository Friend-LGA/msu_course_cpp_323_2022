#include "config.hpp"
#include "graph.hpp"
#include "graph_generation_controller.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "graph_printing.hpp"
#include "logger.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using Graph = uni_course_cpp::Graph;
using GraphGenerator = uni_course_cpp::GraphGenerator;
using Logger = uni_course_cpp::Logger;
using GraphGenerationController = uni_course_cpp::GraphGenerationController;
using GraphDepth = uni_course_cpp::GraphDepth;
using IGraph = uni_course_cpp::IGraph;

GraphDepth handle_depth_input() {
  std::string input_string;
  GraphDepth depth_tmp;
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

int handle_new_vertices_count_input() {
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

GraphDepth handle_graphs_count_input() {
  std::string input_string;
  GraphDepth graphs_count_tmp;
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

int handle_threads_count_input() {
  std::string input_string;
  int threads_count_tmp;
  bool is_alpha_flag = false;
  while (true) {
    std::cout << "Enter threads count: ";
    std::cin >> input_string;
    std::cout << std::endl;
    for (char next_symbol : input_string) {
      if (!isdigit(next_symbol)) {
        is_alpha_flag = true;
        break;
      }
    }
    if (is_alpha_flag) {
      std::cout << "Incorrect data, threads count must be not negative "
                   "integer.\n";
      is_alpha_flag = false;
      continue;
    }
    threads_count_tmp = std::atoi(input_string.c_str());
    break;
  }
  return threads_count_tmp;
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

std::vector<std::unique_ptr<IGraph>> generate_graphs(
    GraphGenerator::Params&& params,
    int graphs_count,
    int threads_count) {
  auto generation_controller =
      GraphGenerationController(threads_count, graphs_count, std::move(params));

  auto& logger = Logger::get_logger();

  auto graphs = std::vector<std::unique_ptr<IGraph>>();
  graphs.reserve(graphs_count);

  generation_controller.generate(
      [&logger](int index) { logger.log(generation_started_string(index)); },
      [&logger, &graphs](int index, std::unique_ptr<IGraph> graph) {
        const auto graph_description =
            uni_course_cpp::printing::print_graph(*graph);
        logger.log(generation_finished_string(index, graph_description));
        const auto graph_json =
            uni_course_cpp::printing::json::print_graph(*graph);
        write_to_file(graph_json, "graph_" + std::to_string(index) + ".json");
        graphs.push_back(std::move(graph));
      });

  return graphs;
}

int main() {
  const int depth = handle_depth_input();
  const int new_vertices_count = handle_new_vertices_count_input();
  const int graphs_count = handle_graphs_count_input();
  const int threads_count = handle_threads_count_input();
  prepare_temp_directory();

  auto params = GraphGenerator::Params(depth, new_vertices_count);
  const auto graphs =
      generate_graphs(std::move(params), graphs_count, threads_count);

  return 0;
}
