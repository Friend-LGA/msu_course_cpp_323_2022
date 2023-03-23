#include <fstream>
#include <iostream>
#include <string>
#include "graph.hpp"

namespace printing {
namespace json {
std::string printVertex(const uni_course_cpp::VertexId& id,
                        const uni_course_cpp::Graph& graph) {
  std::string vertex_string =
      "{\n   \"id\": " + std::to_string(id) + ",\n   \"edge_ids\": [";
  for (const auto& edge_id : graph.vertexConnections(id)) {
    vertex_string += std::to_string(edge_id) + ", ";
  }
  vertex_string.pop_back();
  vertex_string.pop_back();
  vertex_string += "]\n  }, ";
  return vertex_string;
}
std::string printEdge(const uni_course_cpp::Edge& edge) {
  std::string edge_string = "{\n   \"id\": " + std::to_string(edge.id);
  edge_string += ",\n   \"vertex_ids\": [";
  edge_string += std::to_string(edge.from_vertex_id) + ", ";
  edge_string += std::to_string(edge.to_vertex_id) + "]\n  }, ";
  return edge_string;
}

std::string printGraph(const uni_course_cpp::Graph& graph) {
  std::string graph_string;
  graph_string += "{\n \"vertices\": [\n  ";
  for (const auto& vertex : graph.vertexes()) {
    graph_string += printVertex(vertex.id, graph);
  }
  graph_string.pop_back();
  graph_string.pop_back();
  graph_string += "\n ],\n \"edges\": [\n  ";
  for (const auto& edge : graph.edges()) {
    graph_string += printEdge(edge);
  }
  graph_string.pop_back();
  graph_string.pop_back();
  graph_string += "\n ]\n}\n";
  return graph_string;
}

}  // namespace json
}  // namespace printing

constexpr int kVerticesCount = 14;

uni_course_cpp::Graph generateGraph() {
  auto graph = uni_course_cpp::Graph();

  for (int i = 0; i < kVerticesCount; i++) {
    graph.addVertex();
  }

  graph.addEdge(0, 1);
  graph.addEdge(0, 2);
  graph.addEdge(0, 3);
  graph.addEdge(1, 4);
  graph.addEdge(1, 5);
  graph.addEdge(1, 6);
  graph.addEdge(2, 7);
  graph.addEdge(2, 8);
  graph.addEdge(3, 9);
  graph.addEdge(4, 10);
  graph.addEdge(5, 10);
  graph.addEdge(6, 10);
  graph.addEdge(7, 11);
  graph.addEdge(8, 11);
  graph.addEdge(9, 12);
  graph.addEdge(10, 13);
  graph.addEdge(11, 13);
  graph.addEdge(12, 13);
  return graph;
}

void writeToFile(const std::string& string, const std::string& file_name) {
  std::ofstream file(file_name);
  file << string;
  file.close();
}

int main() {
  const auto graph = generateGraph();
  const auto graphJson = printing::json::printGraph(graph);
  writeToFile(graphJson, "graph.json");
  return 0;
}
