#include <filesystem>
#include "config.hpp"
#include "graph.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "logger.hpp"
#include "printing.hpp"

namespace {

void write_to_file(const std::string& graph_json,
                   const std::string& file_name) {
  std::ofstream file(file_name);
  file << graph_json;
}

uni_course_cpp::Graph::Depth handle_depth_input() {
  uni_course_cpp::Graph::Depth depth;
  std::cin >> depth;
  return depth;
}

int handle_new_vertices_count_input() {
  int count_vertices;
  std::cin >> count_vertices;
  return count_vertices;
}

int handle_graphs_count_input() {
  int graph_count = 0;
  while (!(std::cin >> graph_count) || graph_count < 0) {
    std::cout << "Invalid value. Please, try again." << std::endl
              << "Enter graph count: ";
  }
  return graph_count;
}

void prepare_temp_directory() {
  std::filesystem::create_directory(uni_course_cpp::config::kTempDirectoryPath);
}

std::string generation_started_string(int number_of_graph,
                                      uni_course_cpp::Logger& logger) {
  std::stringstream result;
  result << logger.get_current_date_time() << " Graph " << number_of_graph
         << ", Generation Started\n";
  return result.str();
}

std::string generation_finished_string(int number_of_graph,
                                       std::string content,
                                       uni_course_cpp::Logger& logger) {
  std::stringstream result;
  result << logger.get_current_date_time() << " Graph " << number_of_graph
         << ", Generation Finished " << content << "\n";
  return result.str();
}
}  // namespace

int main() {
  const int depth = handle_depth_input();
  const int new_vertices_count = handle_new_vertices_count_input();
  const int graphs_count = handle_graphs_count_input();
  prepare_temp_directory();

  auto params =
      uni_course_cpp::GraphGenerator::Params(depth, new_vertices_count);
  const auto generator = uni_course_cpp::GraphGenerator(std::move(params));
  auto& logger = uni_course_cpp::Logger::get_instance();

  for (int i = 0; i < graphs_count; i++) {
    logger.log(generation_started_string(i, logger));
    const auto graph = generator.generate();

    const auto graph_description = uni_course_cpp::printing::print_graph(graph);
    logger.log(generation_finished_string(i, graph_description, logger));

    const auto graph_json = uni_course_cpp::printing::json::print_graph(graph);
    write_to_file(graph_json, "graph_" + std::to_string(i) + ".json");
  }

  return 0;
}
