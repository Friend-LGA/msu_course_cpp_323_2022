#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "config.hpp"
#include "graph.hpp"
#include "graph_generation_controller.hpp"
#include "graph_generator.hpp"
#include "graph_path.hpp"
#include "graph_printer.hpp"
#include "graph_traversal_controller.hpp"
#include "graph_traverser.hpp"
#include "logger.hpp"

namespace {
int handle_depth_input() {
  int depth;
  std::cout << "Enter depth:" << std::endl;
  std::cin >> depth;
  while (depth < 0) {
    std::cout << "Depth must be not negative. Enter depth again:" << std::endl;
    std::cin >> depth;
  }
  return depth;
}

int handle_new_vertexes_num_input() {
  int new_vertexes_num;
  std::cout << "Enter number of new vertices:" << std::endl;
  std::cin >> new_vertexes_num;
  while (new_vertexes_num < 0) {
    std::cout << "Number of new vertices must be not negative. Enter number of "
                 "new vertexes again:"
              << std::endl;
    std::cin >> new_vertexes_num;
  }
  return new_vertexes_num;
}

int handleNewGraphsCountInput() {
  int new_graphs_num;
  std::cout << "Enter number of new graphs:" << std::endl;
  std::cin >> new_graphs_num;
  while (new_graphs_num < 0) {
    std::cout << "Number of new graphs must be not negative. Enter number of "
                 "new hraphs again:"
              << std::endl;
    std::cin >> new_graphs_num;
  }
  return new_graphs_num;
}

int handleThreadsNum() {
  int threads_num;
  std::cout << "Enter number of threads:" << std::endl;
  std::cin >> threads_num;
  while (threads_num < 1) {
    std::cout << "Number of threads must be positive" << std::endl;
    std::cin >> threads_num;
  }
  return threads_num;
}

void write_to_file(const std::string& string, const std::string& file_name) {
  std::ofstream file(file_name);
  file << string;
  file.close();
}

std::string genStartedString(int i) {
  return "Graph " + std::to_string(i) + ", Generation Started\n";
}

std::string genFinishedString(int i, const uni_course_cpp::Graph& graph) {
  std::string string =
      "Graph " + std::to_string(i) + ", Generation Finished {\n";
  int depth = graph.depth();
  string += " depth: " + std::to_string(depth) + ",\n";
  string += " vertices: " + std::to_string(graph.vertexes().size()) + ", [";
  for (int j = 0; j <= depth; j++) {
    string += std::to_string(graph.vertexIdsAtLayer(j).size()) + ", ";
  }
  string.pop_back();
  string.pop_back();
  string += "],\n";
  string += " edges: " + std::to_string(graph.edges().size()) + ", {";
  for (const auto& color : uni_course_cpp::Edge::ALL_COLORS) {
    string += uni_course_cpp::GraphPrinter::colorToString(color) + ": ";
    string += std::to_string(graph.colorEdges(color).size()) + ", ";
  }
  string.pop_back();
  string.pop_back();
  string += "},\n}\n";
  return string;
}

std::string traversalStartedString(int index) {
  return "Graph" + std::to_string(index) + ", Traversal Started\n";
}

std::string traversalFinishedString(
    int index,
    const std::vector<uni_course_cpp::GraphPath>& paths) {
  std::string string =
      "Graph " + std::to_string(index) + ", Traversal Ended, Paths: {\n";
  for (const auto& path : paths) {
    string += "  vertexes: [";
    for (const auto& vertex_id : path.vertex_ids) {
      string += std::to_string(vertex_id) + ", ";
    }
    string.pop_back();
    string.pop_back();
    string += "], distance: " + std::to_string(path.distance()) + ",\n";
  }
  string += "}\n";
  return string;
}

void prepareTempDirectory() {
  std::filesystem::create_directory(
      uni_course_cpp::config::TEMP_DIRECTORY_PATH);
}

std::vector<uni_course_cpp::Graph> generateGraphs(
    const uni_course_cpp::GraphGenerator::Params& params,
    int graphs_count,
    int threads_count) {
  auto generation_controller = uni_course_cpp::GraphGenerationController(
      threads_count, graphs_count, params);

  auto& logger = uni_course_cpp::Logger::getLogger();

  auto graphs = std::vector<uni_course_cpp::Graph>();
  graphs.reserve(graphs_count);

  generation_controller.generate(
      [&logger](int index) { logger.log(genStartedString(index)); },
      [&logger, &graphs](int index, uni_course_cpp::Graph graph) {
        logger.log(genFinishedString(index, graph));
        const auto graph_printer = uni_course_cpp::GraphPrinter(graph);
        write_to_file(graph_printer.print(),
                      uni_course_cpp::config::TEMP_DIRECTORY_PATH + "graph_" +
                          std::to_string(index) + ".json");
        graphs.push_back(std::move(graph));
      });
  return graphs;
}

void traverseGraphs(const std::vector<uni_course_cpp::Graph>& graphs) {
  auto traversal_controller = uni_course_cpp::GraphTraversalController(graphs);

  auto& logger = uni_course_cpp::Logger::getLogger();

  traversal_controller.traverse(
      [&logger](int index) { logger.log(traversalStartedString(index)); },
      [&logger](int index, std::vector<uni_course_cpp::GraphPath> paths) {
        logger.log(traversalFinishedString(index, paths));
      });
}

}  // namespace

int main() {
  const int depth = handle_depth_input();
  const int new_vertexes_num = handle_new_vertexes_num_input();
  const int graphs_count = handleNewGraphsCountInput();
  const int threads_num = handleThreadsNum();
  prepareTempDirectory();

  const auto params =
      uni_course_cpp::GraphGenerator::Params(depth, new_vertexes_num);
  const auto graphs = generateGraphs(params, graphs_count, threads_num);
  traverseGraphs(graphs);
  return 0;
}
