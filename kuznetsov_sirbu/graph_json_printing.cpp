#include "graph_json_printing.hpp"
#include <stdexcept>

namespace uni_course_cpp {
namespace printing {
namespace json {

std::string print_vertex(const IVertex& vertex, const IGraph& graph) {
  std::string result_json_vertex = "{\"id\":";
  result_json_vertex += std::to_string(vertex.id()) + ",";
  const std::vector<EdgeId>& edge_ids = graph.connected_edges_ids(vertex.id());

  result_json_vertex += "\"edge_ids\":[";
  for (const auto& id : edge_ids) {
    result_json_vertex += std::to_string(id) + ",";
  }

  if (edge_ids.size())
    result_json_vertex.pop_back();
  result_json_vertex += "],";
  result_json_vertex +=
      "\"depth\":" + std::to_string(graph.vertex_depth(vertex.id())) + "}";
  return result_json_vertex;
}

std::string print_edge_color(EdgeColor edge_color) {
  switch (edge_color) {
    case EdgeColor::Grey:
      return "grey";
    case EdgeColor::Red:
      return "red";
    case EdgeColor::Yellow:
      return "yellow";
    case EdgeColor::Green:
      return "green";
  }
  throw std::runtime_error("Failed to determine color");
}

std::string print_edge(const IEdge& edge) {
  std::string result_json_edge =
      "{\"id\":" + std::to_string(edge.id()) + ",\"vertex_ids\":" + "[" +
      std::to_string(edge.from_vertex_id()) + "," +
      std::to_string(edge.to_vertex_id()) + "]," + "\"color\":";
  const std::string edge_color = print_edge_color(edge.color());
  result_json_edge += "\"" + edge_color + "\"" + "}";
  return result_json_edge;
}

std::string print_graph(const IGraph& graph) {
  std::string result = "";
  result += "{\"depth\":" + std::to_string(graph.depth()) + ",";
  result += "\"vertices\":[";

  for (const auto& vertex : graph.get_vertices()) {
    result += print_vertex(*vertex, graph) + ",";
  }
  if (result.back() == ',')
    result.pop_back();
  result += "],\"edges\":[";

  for (const auto& edge : graph.get_edges()) {
    result += print_edge(*edge) + ",";
  }
  if (result.back() == ',')
    result.pop_back();
  result += "]}\n";
  return result;
}
}  // namespace json
}  // namespace printing
}  // namespace uni_course_cpp
