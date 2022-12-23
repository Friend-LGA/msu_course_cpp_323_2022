#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "graph.hpp"
#include "graph_generator.hpp"

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

int handle_new_vertexes_count_input() {
  int new_vertexes_num;
  std::cout << "Enter number of new vertexes:" << std::endl;
  std::cin >> new_vertexes_num;
  while (new_vertexes_num < 0) {
    std::cout << "Number of new vertexes must be not negative. Enter number of "
                 "new vertexes again:"
              << std::endl;
    std::cin >> new_vertexes_num;
  }
  return new_vertexes_num;
}

int handle_new_graphs_count_input() {
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

void write_to_file(const std::string& string, const std::string& file_name) {
  std::ofstream file(file_name);
  file << string;
  file.close();
}

}  // namespace
namespace printing {
std::string colorToString(const uni_course_cpp::Edge::Color& color) {
  switch (color) {
    case uni_course_cpp::Edge::Color::Gray:
      return "gray";
    case uni_course_cpp::Edge::Color::Green:
      return "green";
    case uni_course_cpp::Edge::Color::Yellow:
      return "yellow";
    case uni_course_cpp::Edge::Color::Red:
      return "red";
    default:
      throw std::runtime_error("Cannot be reached.");
  }
}

std::string printVertex(const uni_course_cpp::VertexId& id,
                        const uni_course_cpp::Graph& graph) {
  std::string vertex_string =
      "{\n   \"id\": " + std::to_string(id) + ",\n   \"edge_ids\": [";
  for (const auto& edge_id : graph.vertexConnections(id)) {
    vertex_string += std::to_string(edge_id) + ", ";
  }
  if (graph.vertexConnections(id).size() > 0) {
    vertex_string.pop_back();
    vertex_string.pop_back();
  }
  vertex_string +=
      "],\n   \"depth\": " + std::to_string(graph.vertexDepth(id)) + "\n  }, ";
  return vertex_string;
}
std::string printEdge(const uni_course_cpp::Edge& edge) {
  std::string edge_string = "{\n   \"id\": " + std::to_string(edge.id);
  edge_string += ",\n   \"vertex_ids\": [";
  edge_string += std::to_string(edge.from_vertex_id) + ", ";
  edge_string += std::to_string(edge.to_vertex_id) + "],\n   ";
  edge_string += "\"color\": \"" + colorToString(edge.color) + "\"\n  }, ";
  return edge_string;
}

std::string print_graph(const uni_course_cpp::Graph& graph, int depth) {
  std::string graph_string;
  graph_string += "\n\"depth\":";
  graph_string += std::to_string(depth);
  graph_string += "{\n \"vertexes\": [\n  ";
  for (const auto& vertex : graph.vertexes()) {
    graph_string += printVertex(vertex.id, graph);
  }
  if (graph.vertexes().size() > 0) {
    graph_string.pop_back();
    graph_string.pop_back();
  }
  graph_string += "\n ],\n \"edges\": [\n  ";
  for (const auto& edge : graph.edges()) {
    graph_string += printEdge(edge);
  }
  if (graph.edges().size() > 0) {
    graph_string.pop_back();
    graph_string.pop_back();
  }
  graph_string += "\n ]\n}\n";
  return graph_string;
}

}  // namespace printing

int main() {
  const int depth = handle_depth_input();
  const int new_vertexes_count = handle_new_vertexes_count_input();
  auto params =  uni_course_cpp::GraphGenerator::Params(depth,new_vertexes_count );
  const auto generator = uni_course_cpp::GraphGenerator(std::move(params));
  const auto graph = generator.generate();
  const auto graphjson = printing::print_graph(graph, depth);
  write_to_file(graphjson, "graph.json");
  return 0;
}
