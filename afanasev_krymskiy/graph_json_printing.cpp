#include <string>
#include <unordered_map>
#include <vector>

#include "graph.hpp"
#include "graph_json_printing.hpp"
#include "graph_printing.hpp"
#include "interfaces/i_graph.hpp"
#include "interfaces/i_vertex.hpp"

namespace uni_course_cpp {
std::string printing::json::print_vertex(const IVertex& vertex,
                                         const IGraph& graph) {
  std::string result = "{";

  result += "\"id\":";
  result += std::to_string(vertex.id());

  result += ",";

  result += "\"edge_ids\":[";
  const auto& edge_ids = graph.connected_edge_ids(vertex.id());
  for (const auto edge_id : edge_ids) {
    result += std::to_string(edge_id);
    result += ",";
  }
  if (!edge_ids.empty()) {
    result.pop_back();
  }
  result += "]";

  result += ",";

  result += "\"depth\": ";
  result += std::to_string(graph.vertex_depth(vertex.id()));

  result += "}";
  return result;
}

std::string printing::json::print_edge(const IEdge& edge) {
  std::string result = "{";

  result += "\"id\":";
  result += std::to_string(edge.id());

  result += ",";

  result += "\"vertex_ids\":[";
  result += std::to_string(edge.from_vertex_id());
  result += ",";
  result += std::to_string(edge.to_vertex_id());
  result += "]";

  result += ",";

  result += "\"color\": \"";
  result += print_edge_color(edge.color());
  result += "\"";

  result += "}";
  return result;
}

std::string printing::json::print_graph(const IGraph& graph) {
  std::string result = "{";

  result += "\"depth\": ";
  result += std::to_string(graph.depth());

  result += ",";

  result += "\"vertices\":[";
  const auto& vertices = graph.vertices();
  for (const auto& [vertex_id, vertex] : vertices) {
    result += print_vertex(*vertex, graph);
    result += ",";
  }
  if (!vertices.empty()) {
    result.pop_back();
  }

  result += "]";

  result += ",";

  result += "\"edges\":[";
  const auto& edges = graph.edges();
  for (const auto& [edge_id, edge] : edges) {
    result += print_edge(*edge);
    result += ",";
  }
  if (!edges.empty()) {
    result.pop_back();
  }

  result += "]";

  result += "}\n";
  return result;
}
}  // namespace uni_course_cpp
