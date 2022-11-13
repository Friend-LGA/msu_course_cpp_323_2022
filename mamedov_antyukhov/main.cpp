#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "config.hpp"
#include "graph.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "graph_printing.hpp"
#include "logger.hpp"

namespace uni_course_cpp {
int handle_depth_input() {
  std::string input;
  int output, tries = 0;
  constexpr int max_tries_num = 15;
  std::cout << "\nEnter the estimated depth of the graph (depth is a "
               "non-negative integer):"
            << std::endl;
  while (tries < max_tries_num && std::cin >> input) {
    ++tries;
    if (sscanf(input.c_str(), "%d", &output)) {
      if (output >= 0)
        return output;
    }
    if (!(tries % 5))
      std::cout << "Read the description carefully!";
    else
      std::cout << "Incorrect depth!";
    std::cout << " Try one more time." << std::endl;
  }
  throw std::runtime_error("Depth has not been passed!");
  return output;
}

int handle_new_vertices_count_input() {
  std::string input;
  int output, tries = 0;
  constexpr int max_tries_num = 15;
  std::cout
      << "\nEnter \"new_vertices_count\" variable (new_vertices_count is a "
         "non-negative integer):"
      << std::endl;
  while (tries < max_tries_num && std::cin >> input) {
    ++tries;
    if (sscanf(input.c_str(), "%d", &output)) {
      if (output >= 0)
        return output;
    }
    if (!(tries % 5))
      std::cout << "Read the description carefully!";
    else
      std::cout << "Incorrect new_vertices_count variable!";
    std::cout << " Try one more time." << std::endl;
  }

  throw std::runtime_error("New_vertices_count has been passed!");
  return output;
}

int handle_graphs_count_input() {
  std::string input;
  int output, tries = 0;
  constexpr int max_tries_num = 15;
  std::cout << "\n Enter \"graphs_count\" variable (graphs_count is a "
               "non-negative integer):"
            << std::endl;
  while (tries < max_tries_num && std::cin >> input) {
    ++tries;
    if (sscanf(input.c_str(), "%d", &output)) {
      if (output >= 0)
        return output;
    }
    if (!(tries % 5))
      std::cout << "Read the description carefully!";
    else
      std::cout << "Incorrect graphs_count variable!";
    std::cout << "Try one more time." << std::endl;
  }

  throw std::runtime_error("New_graph_count has been passed!");

  return output;
}

void write_to_file(const std::string& graph_str, const std::string& file_name) {
  std::ofstream fout(file_name);
  fout << graph_str;
}

void prepare_temp_directory() {
  if (!std::filesystem::create_directory("temp"))
    throw std::runtime_error("Can not create temp directory");
  return;
}

std::string generation_started_string(int graph_num) {
  std::stringstream started_string;
  started_string << "Graph " << graph_num << ", Generation Started\n";
  return started_string.str();
  ;
}

std::string generation_finished_string(int graph_num,
                                       const std::string& graph_description) {
  std::stringstream finished_string;
  finished_string << "Graph " << graph_num << ", Generation Finished "
                  << graph_description << std::endl;
  return finished_string.str();
}

}  // namespace uni_course_cpp

int main() {
  const int depth = uni_course_cpp::handle_depth_input();
  const int new_vertices_count =
      uni_course_cpp::handle_new_vertices_count_input();
  const int graphs_count = uni_course_cpp::handle_graphs_count_input();
  uni_course_cpp::prepare_temp_directory();

  auto params =
      uni_course_cpp::GraphGenerator::Params(depth, new_vertices_count);
  const auto generator = uni_course_cpp::GraphGenerator(std::move(params));
  auto& logger = uni_course_cpp::Logger::get_logger();

  for (int i = 0; i < graphs_count; ++i) {
    logger.log(uni_course_cpp::generation_started_string(i));
    const auto graph = generator.generate();

    const auto& graph_description =
        uni_course_cpp::printing::print_graph(graph);
    logger.log(
        uni_course_cpp::generation_finished_string(i, graph_description));

    const auto graph_json = uni_course_cpp::printing::json::print_graph(graph);
    uni_course_cpp::write_to_file(
        graph_json, std::string(uni_course_cpp::config::kTempDirectoryPath) +
                        "graph_" + std::to_string(i) + ".json");
  }

  return 0;
}
