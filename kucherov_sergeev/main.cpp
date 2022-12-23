#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "config.hpp"
#include "graph.hpp"
#include "graph_generation_controller.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "graph_printing.hpp"
#include "logger.hpp"

using Graph = uni_course_cpp::Graph;
using GraphGenerator = uni_course_cpp::GraphGenerator;
using Logger = uni_course_cpp::Logger;

void write_to_file(const std::string& graph_json,
                   const std::string& file_name) {
  const std::string file_path =
      uni_course_cpp::config::kTempDirectoryPath + file_name;
  std::ofstream json_file(file_path);

  json_file << graph_json;
}

int handle_depth_input() {
  const std::string init_message = "Type graph depth: ";
  const std::string err_format_message =
      "The depth must be a non-negative integer. Try again";

  int depth;
  int correct_input = false;

  std::cout << init_message << std::endl;

  while (correct_input == false) {
    if (std::cin >> depth && depth >= 0) {
      correct_input = true;
    } else if (std::cin.fail() || depth < 0) {
      std::cout << err_format_message << std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
      throw std::runtime_error("Failed to get depth from user");
    }
  }

  return depth;
}

int handle_new_vertices_count_input() {
  const std::string init_message = "Type new vertices count: ";
  const std::string err_format_message =
      "Vertices count must be a non-negative integer. Try again";
  int new_vertices_count;
  int correct_input = false;

  std::cout << init_message << std::endl;

  while (correct_input == false) {
    if (std::cin >> new_vertices_count && new_vertices_count >= 0) {
      correct_input = true;
    } else if (std::cin.fail() || new_vertices_count < 0) {
      std::cout << err_format_message << std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
      throw std::runtime_error("Failed to get new vertices count from user");
    }
  }

  return new_vertices_count;
}

int handle_graphs_count_input() {
  const std::string init_message = "Type new graphs count: ";
  const std::string err_format_message =
      "Graphs count must be a non-negative integer. Try again";
  int graphs_count;
  int correct_input = false;

  std::cout << init_message << std::endl;

  while (correct_input == false) {
    if (std::cin >> graphs_count && graphs_count >= 0) {
      correct_input = true;
    } else if (std::cin.fail() || graphs_count < 0) {
      std::cout << err_format_message << std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
      throw std::runtime_error("Failed to get new graphs count from user");
    }
  }

  return graphs_count;
}

int handle_threads_count_input() {
  const std::string init_message = "Type threads count: ";
  const std::string err_format_message =
      "Threads count must be a positive integer. Try again";
  int threads_count;
  int correct_input = false;

  std::cout << init_message << std::endl;

  while (correct_input == false) {
    if (std::cin >> threads_count && threads_count >= 1) {
      correct_input = true;
    } else if (std::cin.fail() || threads_count < 1) {
      std::cout << err_format_message << std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
      throw std::runtime_error("Failed to get threads count from user");
    }
  }

  return threads_count;
}

std::string generation_started_string(int graph_number) {
  return "Graph " + std::to_string(graph_number) + ", Generation Started";
}

std::string generation_finished_string(int graph_number,
                                       const std::string& graph_description) {
  return "Graph " + std::to_string(graph_number) + ", Generation Finished " +
         graph_description;
}

void prepare_temp_directory() {
  if (std::filesystem::exists(uni_course_cpp::config::kTempDirectoryPath) ==
      false) {
    if (std::filesystem::create_directory(
            uni_course_cpp::config::kTempDirectoryPath) == false) {
      throw std::runtime_error("Can't prepare temp directory");
    }
  }
}

std::vector<Graph> generate_graphs(GraphGenerator::Params&& params,
                                   int graphs_count,
                                   int threads_count) {
  auto generation_controller = uni_course_cpp::GraphGenerationController(
      threads_count, graphs_count, std::move(params));

  auto& logger = Logger::get_logger();

  auto graphs = std::vector<Graph>();
  graphs.reserve(graphs_count);

  generation_controller.generate(
      [&logger](int index) { logger.log(generation_started_string(index)); },
      [&logger, &graphs](int index, Graph&& graph) {
        graphs.push_back(graph);
        const auto graph_description =
            uni_course_cpp::printing::print_graph(graph);
        logger.log(generation_finished_string(index, graph_description));
        const auto graph_json =
            uni_course_cpp::printing::json::print_graph(graph);
        write_to_file(graph_json, "graph_" + std::to_string(index) + ".json");
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
