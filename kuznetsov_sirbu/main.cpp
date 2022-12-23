#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "config.hpp"
#include "graph.hpp"
#include "graph_generation_controller.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "interfaces/i_graph.hpp"
#include "logger.hpp"
#include "printing.hpp"

namespace {

void write_to_file(const std::string& graph_json,
                   const std::string& file_name) {
  std::ofstream file(uni_course_cpp::config::kTempDirectoryPath + file_name);
  file << graph_json;
}

constexpr int kInputSize = 256;

uni_course_cpp::GraphDepth handle_depth_input() {
  std::cout << "Depth: ";
  int depth = 0;
  while (!(std::cin >> depth) || depth < 0) {
    std::cout << "Invalid value" << std::endl << "Depth: ";
    std::cin.clear();
    std::cin.ignore(kInputSize, '\n');
  }
  return depth;
}

int handle_new_vertices_count_input() {
  std::cout << "Vertices count: ";
  int new_vertices_count = 0;
  while (!(std::cin >> new_vertices_count) || new_vertices_count < 0) {
    std::cout << "Invalid value" << std::endl << "Vertices count: ";
    std::cin.clear();
    std::cin.ignore(kInputSize, '\n');
  }
  return new_vertices_count;
}

int handle_graphs_count_input() {
  std::cout << "Graphs count: ";
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
  std::cout << "Threads count: ";
  int threads_count = 0;
  while (!(std::cin >> threads_count) || threads_count < 0) {
    std::cout << "Invalid value. Please, try again." << std::endl
              << "Threads count: ";
    std::cin.clear();
    std::cin.ignore(kInputSize, '\n');
  }
  return threads_count;
}

void prepare_temp_directory() {
  if (std::filesystem::exists(uni_course_cpp::config::kTempDirectoryPath))
    return;
  if (!std::filesystem::create_directory(
          uni_course_cpp::config::kTempDirectoryPath)) {
    throw std::runtime_error("Can not create temp directory");
  }
}

std::string generation_started_string(int number_of_graph) {
  std::stringstream result;
  result << " Graph " << number_of_graph << ", Generation Started\n";
  return result.str();
}

std::string generation_finished_string(int number_of_graph,
                                       const std::string& content) {
  std::stringstream result;
  result << " Graph " << number_of_graph << ", Generation Finished " << content
         << "\n";
  return result.str();
}

std::vector<std::unique_ptr<uni_course_cpp::IGraph>> generate_graphs(
    uni_course_cpp::GraphGenerator::Params&& params,
    int graphs_count,
    int threads_count) {
  auto generation_controller = uni_course_cpp::GraphGenerationController(
      threads_count, graphs_count, std::move(params));

  auto& logger = uni_course_cpp::Logger::get_instance();

  auto graphs = std::vector<std::unique_ptr<uni_course_cpp::IGraph>>();
  graphs.reserve(graphs_count);
  generation_controller.generate(
      [&logger](int index) { logger.log(generation_started_string(index)); },
      [&logger, &graphs](int index,
                         std::unique_ptr<uni_course_cpp::IGraph> graph) {
        const auto graph_description =
            uni_course_cpp::printing::print_graph(*graph);
        graphs.push_back(std::move(graph));
        logger.log(generation_finished_string(index, graph_description));
        const auto graph_json =
            uni_course_cpp::printing::json::print_graph(*graphs.back());
        write_to_file(graph_json, "graph_" + std::to_string(index) + ".json");
      });

  return graphs;
}

}  // namespace

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
