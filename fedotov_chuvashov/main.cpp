#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>
#include "config.hpp"
#include "graph.hpp"
#include "graph_generation_controller.hpp"
#include "graph_generator.hpp"
#include "logger.hpp"
#include "printing.hpp"
#include "printing_json.hpp"

void write_to_file(const std::string& output_string,
                   const std::string& file_name) {
  std::ofstream output(file_name);
  if (output.is_open()) {
    output << output_string;
  } else {
    std::cout << "Unable to open file";
  }
}

constexpr int kInputSize = 256;

int handle_depth_input() {
  std::cout << "Enter depth: ";
  int depth = 0;
  while (!(std::cin >> depth) || depth < 0) {
    std::cout << "Invalid value. Please, try again." << std::endl
              << "Enter depth: ";
    std::cin.clear();
    std::cin.ignore(kInputSize, '\n');
  }
  return depth;
}

int handle_new_vertices_count_input() {
  std::cout << "Enter new vertices count: ";
  int new_vertices_count = 0;
  while (!(std::cin >> new_vertices_count) || new_vertices_count < 0) {
    std::cout << "Invalid value. Please, try again." << std::endl
              << "Enter new vertices count: ";
    std::cin.clear();
    std::cin.ignore(kInputSize, '\n');
  }
  return new_vertices_count;
}

int handle_graphs_count_input() {
  std::cout << "Enter graph count: ";
  int graph_count = 0;
  while (!(std::cin >> graph_count) || graph_count < 0) {
    std::cout << "Invalid value. Please, try again." << std::endl
              << "Enter graph count: ";
    std::cin.clear();
    std::cin.ignore(kInputSize, '\n');
  }
  return graph_count;
}

int handle_threads_count_input() {
  std::cout << "Enter thread count: ";
  int thread_count = 0;
  while (!(std::cin >> thread_count) || thread_count < 0) {
    std::cout << "Invalid value. Please, try again." << std::endl
              << "Enter thread count: ";
    std::cin.clear();
    std::cin.ignore(kInputSize, '\n');
  }
  return thread_count;
}

std::string generation_started_string(int num_of_graph) {
  std::stringstream start_string;
  start_string << " Graph " << num_of_graph << ", Generation Started\n";
  return start_string.str();
}

std::string generation_finished_string(int num_of_graph,
                                       const std::string& content) {
  std::stringstream finish_string;
  finish_string << " Graph " << num_of_graph << ", Generation Finished "
                << content << "\n";
  return finish_string.str();
}

void prepare_temp_directory() {
  std::filesystem::create_directory(uni_course_cpp::config::kTempDirectoryPath);
}

std::vector<uni_course_cpp::Graph> generate_graphs(
    uni_course_cpp::GraphGenerator::Params&& params,
    int graphs_count,
    int threads_count) {
  auto generation_controller = uni_course_cpp::GraphGenerationController(
      threads_count, graphs_count, std::move(params));

  auto& logger = uni_course_cpp::Logger::get_logger();

  auto graphs = std::vector<uni_course_cpp::Graph>();
  graphs.reserve(graphs_count);

  generation_controller.generate(
      [&logger](int index) { logger.log(generation_started_string(index)); },
      [&logger, &graphs](int index, uni_course_cpp::Graph&& graph) {
        graphs.push_back(graph);
        const auto graph_description =
            uni_course_cpp::printing::print_graph(graph);
        logger.log(generation_finished_string(index, graph_description));
        const auto graph_json = uni_course_cpp::json::print_graph(graph);
        write_to_file(graph_json,
                      std::string{uni_course_cpp::config::kTempDirectoryPath} +
                          "graph_" + std::to_string(index) + ".json");
      });

  return graphs;
}

int main() {
  const int depth = handle_depth_input();
  const int new_vertices_count = handle_new_vertices_count_input();
  const int graphs_count = handle_graphs_count_input();
  const int threads_count = handle_threads_count_input();
  prepare_temp_directory();

  auto params =
      uni_course_cpp::GraphGenerator::Params(depth, new_vertices_count);
  const auto graphs =
      generate_graphs(std::move(params), graphs_count, threads_count);

  return 0;
}
