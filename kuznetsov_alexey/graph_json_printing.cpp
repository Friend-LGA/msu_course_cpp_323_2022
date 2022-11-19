#include "graph_json_printing.hpp"

namespace uni_course_cpp {

std::string printing::json::print_vertex(const Graph::Vertex& vertex,
                                         const Graph& graph) {
  std::string result_json_vertex = "{\"id\":";
  result_json_vertex += std::to_string(vertex.id()) + ",";
  const std::vector<Graph::EdgeId>& edge_ids =
      graph.connected_edges_ids(vertex.id());

  result_json_vertex += "\"edge_ids\":[";
  for (const auto& id : edge_ids) {
    result_json_vertex += std::to_string(id) + ",";
  }

  if (edge_ids.size())
    result_json_vertex.pop_back();
  result_json_vertex += "],";
  result_json_vertex += "\"depth\":" + std::to_string(vertex.depth()) + "}";
  return result_json_vertex;
}

std::string printing::json::print_edge(const Graph::Edge& edge) {
  std::string result_json_edge =
      "{\"id\":" + std::to_string(edge.id()) + ",\"vertex_ids\":" + "[" +
      std::to_string(edge.from_vertex_id()) + "," +
      std::to_string(edge.to_vertex_id()) + "]," + "\"color\":";
  std::string edge_color;
  switch (edge.color()) {
    case Graph::Edge::Color::Grey:
      edge_color = "grey";
      break;
    case Graph::Edge::Color::Red:
      edge_color = "red";
      break;
    case Graph::Edge::Color::Yellow:
      edge_color = "yellow";
      break;
    case Graph::Edge::Color::Green:
      edge_color = "green";
      break;
  }
  result_json_edge += "\"" + edge_color + "\"" + "}";
  return result_json_edge;
}

std::string printing::json::print_graph(const Graph& graph) {
  std::string result = "";
  result += "{\"depth\":" + std::to_string(graph.depth()) + ",";
  result += "\"vertices\":[";

  for (const auto& vertex : graph.get_vertices()) {
    result += print_vertex(vertex, graph) + ",";
  }
  if (result.back() == ',')
    result.pop_back();
  result += "],\"edges\":[";

  for (const auto& edge : graph.get_edges()) {
    result += print_edge(edge) + ",";
  }
  if (result.back() == ',')
    result.pop_back();
  result += "]}\n";
  return result;
}

}  // namespace uni_course_cpp
