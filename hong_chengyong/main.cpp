#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "config.hpp"
#include "graph.hpp"
#include "graph_generator.hpp"
#include "graph_printer.hpp"
#include "logger.hpp"

namespace {
int handleDepthInput() {
  int depth;
  std::cout << "Enter depth:" << std::endl;
  std::cin >> depth;
  while (depth < 0) {
    std::cout << "Depth must be not negative. Enter depth again:" << std::endl;
    std::cin >> depth;
  }
  return depth;
}

int handleNewVertexesNumInput() {
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

void writeToFile(const std::string& string, const std::string& file_name) {
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

void prepareTempDirectory() {
  std::filesystem::create_directory(config::TEMP_DIRECTORY_PATH);
}
}  // namespace

int main() {
  const int depth = handleDepthInput();
  const int new_vertexes_num = handleNewVertexesNumInput();
  const int graphs_count = handleNewGraphsCountInput();
  prepareTempDirectory();

  const auto params =
      uni_course_cpp::GraphGenerator::Params(depth, new_vertexes_num);
  const auto generator = uni_course_cpp::GraphGenerator(params);

  auto& logger = uni_course_cpp::Logger::getLogger();

  for (int i = 0; i < graphs_count; i++) {
    logger.log(genStartedString(i));
    const auto graph = generator.generate();
    logger.log(genFinishedString(i, graph));

    const auto graph_printer = uni_course_cpp::GraphPrinter(graph);
    writeToFile(graph_printer.print(), config::TEMP_DIRECTORY_PATH + "graph_" +
                                           std::to_string(i) + ".json");
  }

  return 0;
}
