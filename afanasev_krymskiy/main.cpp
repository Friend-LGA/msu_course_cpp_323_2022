#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "config.hpp"
#include "graph_generation_controller.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "graph_printing.hpp"
#include "interfaces/i_graph.hpp"
#include "logger.hpp"

namespace file_system = std::filesystem;

void write_to_file(const std::string& string, const std::string& file_name) {
  std::ofstream file(file_name);
  file << string;
}

void print_negative_error(const std::string& string) {
  std::cout << string
            << " cannot be negative! Re-enter the value:" << std::endl;
}

void print_zero_error(const std::string& string) {
  std::cout << string << " cannot be zero! Re-enter the value:" << std::endl;
}

void print_error(const std::string& string) {
  std::cout << "Invalid " << string
            << "! (should be integer value [0, "
               "INT_MAX]). Re-enter the value:"
            << std::endl;
}

int handle_depth_input() {
  std::cout << "Enter depth:" << std::endl;
  std::string input_data;
  int depth;
  while (std::getline(std::cin, input_data)) {
    int flag = 1;
    for (const auto symbol : input_data) {
      if (!std::isspace(symbol) && !std::isdigit(symbol) && symbol != '-') {
        print_error("depth");
        flag = 0;
        break;
      }
    }
    if (flag) {
      try {
        depth = std::stoi(input_data);
      } catch (...) {
        print_error("depth");
        continue;
      }
      if (depth < 0) {
        print_negative_error("Depth");
      } else {
        break;
      }
    }
  }
  return depth;
}

int handle_new_vertices_count_input() {
  std::cout << "Enter new_vertices_count:" << std::endl;
  std::string input_data;
  int new_vertices_count;
  while (std::getline(std::cin, input_data)) {
    int flag = 1;
    for (const auto symbol : input_data) {
      if (!std::isspace(symbol) && !std::isdigit(symbol) && symbol != '-') {
        print_error("new_vertices_count");
        flag = 0;
        break;
      }
    }
    if (flag) {
      try {
        new_vertices_count = std::stoi(input_data);
      } catch (...) {
        print_error("new_vertices_count");
        continue;
      }
      if (new_vertices_count < 0) {
        print_negative_error("new_vertices_count");
      } else {
        break;
      }
    }
  }
  return new_vertices_count;
}

int handle_graphs_count_input() {
  std::cout << "Enter graphs_count:" << std::endl;
  std::string input_data;
  int graphs_count;
  while (std::getline(std::cin, input_data)) {
    int flag = 1;
    for (const auto symbol : input_data) {
      if (!std::isspace(symbol) && !std::isdigit(symbol) && symbol != '-') {
        print_error("graphs_count");
        flag = 0;
        break;
      }
    }
    if (flag) {
      try {
        graphs_count = std::stoi(input_data);
      } catch (...) {
        print_error("graphs_count");
        continue;
      }
      if (graphs_count < 0) {
        print_negative_error("graphs_count");
      } else {
        break;
      }
    }
  }
  return graphs_count;
}

int handle_threads_count_input() {
  std::cout << "Enter threads_count:" << std::endl;
  std::string input_data;
  int threads_count;
  while (std::getline(std::cin, input_data)) {
    int flag = 1;
    for (const auto symbol : input_data) {
      if (!std::isspace(symbol) && !std::isdigit(symbol) && symbol != '-') {
        print_error("threads_count");
        flag = 0;
        break;
      }
    }
    if (flag) {
      try {
        threads_count = std::stoi(input_data);
      } catch (...) {
        print_error("threads_count");
        continue;
      }
      if (threads_count < 0) {
        print_negative_error("threads_count");
      } else if (threads_count == 0) {
        print_zero_error("threads_count");
      } else {
        break;
      }
    }
  }
  return threads_count;
}

void prepare_temp_directory() {
  if (!file_system::is_directory(uni_course_cpp::config::kTempDirectoryPath) ||
      !file_system::exists(uni_course_cpp::config::kTempDirectoryPath)) {
    if (!file_system::create_directory(
            uni_course_cpp::config::kTempDirectoryPath)) {
      throw std::runtime_error("Can't create directory!");
    }
  }
}

std::string generation_started_string(int graph_number) {
  return " Graph " + std::to_string(graph_number) + ", Generation Started";
}

std::string generation_finished_string(int graph_number,
                                       const std::string& graph_description) {
  return " Graph " + std::to_string(graph_number) + ", Generation Finished " +
         graph_description;
}

std::vector<std::unique_ptr<uni_course_cpp::IGraph>> generate_graphs(
    uni_course_cpp::GraphGenerator::Params&& params,
    int graphs_count,
    int threads_count) {
  auto generation_controller = uni_course_cpp::GraphGenerationController(
      threads_count, graphs_count, std::move(params));

  auto& logger = uni_course_cpp::Logger::get_logger();

  auto graphs = std::vector<std::unique_ptr<uni_course_cpp::IGraph>>();
  graphs.reserve(graphs_count);

  generation_controller.generate(
      [&logger](int index) { logger.log(generation_started_string(index)); },
      [&logger, &graphs](int index,
                         std::unique_ptr<uni_course_cpp::IGraph> graph) {
        const auto graph_description =
            uni_course_cpp::printing::print_graph(*graph);
        logger.log(generation_finished_string(index, graph_description));

        const auto graph_json =
            uni_course_cpp::printing::json::print_graph(*graph);
        write_to_file(graph_json, uni_course_cpp::config::kTempDirectoryPath +
                                      std::string("graph_") +
                                      std::to_string(index) + ".json");
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

  auto params =
      uni_course_cpp::GraphGenerator::Params(depth, new_vertices_count);
  const auto graphs =
      generate_graphs(std::move(params), graphs_count, threads_count);

  return 0;
}
