#include "graph_printer.hpp"
#include <stdexcept>
#include <string>
#include "graph.hpp"

namespace uni_course_cpp {

std::string GraphPrinter::print() const {
  std::string graph_string;
  graph_string += "{\n \"vertices\": [\n  ";
  for (const auto& vertex : graph_.vertexes()) {
    graph_string += printVertex(vertex.id);
  }
  if (graph_.vertexes().size() > 0) {
    graph_string.pop_back();
    graph_string.pop_back();
  }
  graph_string += "\n ],\n \"edges\": [\n  ";
  for (const auto& edge : graph_.edges()) {
    graph_string += printEdge(edge);
  }
  if (graph_.edges().size() > 0) {
    graph_string.pop_back();
    graph_string.pop_back();
  }
  graph_string += "\n ]\n}\n";
  return graph_string;
}
std::string GraphPrinter::printVertex(const VertexId& id) const {
  std::string vertex_string =
      "{\n   \"id\": " + std::to_string(id) + ",\n   \"edge_ids\": [";
  for (const auto& edge_id : graph_.vertexConnections(id)) {
    vertex_string += std::to_string(edge_id) + ", ";
  }
  if (graph_.vertexConnections(id).size() > 0) {
    vertex_string.pop_back();
    vertex_string.pop_back();
  }
  vertex_string +=
      "],\n   \"depth\": " + std::to_string(graph_.vertexDepth(id)) + "\n  }, ";
  return vertex_string;
}
std::string GraphPrinter::printEdge(const Edge& edge) const {
  std::string edge_string = "{\n   \"id\": " + std::to_string(edge.id);
  edge_string += ",\n   \"vertex_ids\": [";
  edge_string += std::to_string(edge.vertex_id1) + ", ";
  edge_string += std::to_string(edge.vertex_id2) + "],\n   ";
  edge_string += "\"color\": \"" + colorToString(edge.color) + "\"\n  }, ";
  return edge_string;
}

std::string GraphPrinter::colorToString(const Edge::Colors& color) {
  switch (color) {
    case Edge::Colors::Gray:
      return "gray";
    case Edge::Colors::Green:
      return "green";
    case Edge::Colors::Yellow:
      return "yellow";
    case Edge::Colors::Red:
      return "red";
    default:
      throw std::runtime_error("Cannot be reached.");
  }
}
}  // namespace uni_course_cpp