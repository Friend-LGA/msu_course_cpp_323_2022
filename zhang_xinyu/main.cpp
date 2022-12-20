#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "config.hpp"
#include "graph.hpp"
#include "graph_generation_controller.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "graph_logger.hpp"
#include "graph_printing.hpp"

namespace fs = std::filesystem;

std::string generation_started_string(int graph_id) {
  return "Graph " + std::to_string(graph_id) + ", Generation Started";
}

std::string generation_finished_string(int graph_id,
                                       const std::string& graph_description) {
  return "Graph " + std::to_string(graph_id) + ", Generation Finished {\n" +
         graph_description + "\n}";
}

void write_to_file(const std::string& str, const std::string& filename) {
  std::ofstream out(filename);
  out << str << std::endl;
}

bool is_number(const std::string& str) {
  for (int i = 0; i < str.length(); i++) {
    if (isdigit(str[i]) == false) {
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

int handle_graphs_count_input() {
  std::string input;
  do {
    std::cout << "Input graphs count: ";
    std::cin >> input;
    if (is_number(input) && std::stoi(input) >= 0) {
      return std::stoi(input);
    }

    std::cout << "Incorrect graphs count!" << std::endl;
    std::cout << "Input graphs count: ";
  } while (true);
}

int handle_threads_count_input() {
  std::string input;
  do {
    std::cout << "Input threads count: ";
    std::cin >> input;
    if (is_number(input) && std::stoi(input) >= 1) {
      return std::stoi(input);
    }

    std::cout << "Incorrect graphs count!" << std::endl;
    std::cout << "Input threads count: ";
  } while (true);
}

void prepare_temp_directory() {
  if (!fs::exists(config::kTempDirectoryPath)) {
    if (!fs::create_directories(config::kTempDirectoryPath)) {
      throw std::runtime_error("Temp directory error!");
    }
  }
}

std::vector<uni_course_cpp::Graph> generate_graphs(
    uni_course_cpp::GraphGenerator::Params&& params,
    int graphs_count,
    int threads_count) {
  using namespace uni_course_cpp;
  auto generation_controller =
      GraphGenerationController(threads_count, graphs_count, std::move(params));

  auto& logger = Logger::get_logger();

  auto graphs = std::vector<Graph>();
  graphs.reserve(graphs_count);

  generation_controller.generate(
      [&logger](int index) { logger.log(generation_started_string(index)); },
      [&logger, &graphs](int index, Graph&& graph) {
        graphs.push_back(graph);
        const auto graph_description = printing::print_graph(graph);
        logger.log(generation_finished_string(index, graph_description));
        const auto graph_json = printing::json::print_graph(graph);
        write_to_file(graph_json, std::string(config::kTempDirectoryPath) +
                                      "graph_" + std::to_string(index) +
                                      ".json");
      });

  return graphs;
}

int main() {
  using namespace uni_course_cpp;

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
