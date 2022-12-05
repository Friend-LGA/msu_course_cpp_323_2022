#include "graph_json_printing.hpp"
#include "graph_printing.hpp"

std::string printing::json::print_vertex(const Graph::Vertex& vertex,
                                         const Graph& graph) {
  const auto vertex_id = vertex.id();
  std::string vertex_json =
      "{\"id\":" + std::to_string(vertex_id) + ",\"edge_ids\":[";
  const auto& connected_edge_ids = graph.connected_edge_ids(vertex_id);

  for (const auto& edge_id : connected_edge_ids) {
    vertex_json += std::to_string(edge_id) + ",";
  }

  if (!connected_edge_ids.empty()) {
    vertex_json.pop_back();
  }

  vertex_json +=
      "],\"depth\":" + std::to_string(graph.get_vertex_depth(vertex_id)) + "}";

  return vertex_json;
}

std::string printing::json::print_edge(const Graph::Edge& edge) {
  const auto edge_id = edge.id();
  return "{\"id\":" + std::to_string(edge_id) + ",\"vertex_ids\":[" +
         std::to_string(edge.from_vertex_id()) + "," +
         std::to_string(edge.to_vertex_id()) + "],\"color\":\"" +
         printing::print_edge_color(edge.color()) + "\"}";
}

std::string printing::json::print_graph(const Graph& graph) {
  std::string graph_json =
      "{\"depth\":" + std::to_string(graph.get_graph_depth()) +
      ",\"vertices\":[";
  const auto& vertices = graph.vertices();

  if (!vertices.empty()) {
    for (const auto& [vertex_id, vertex] : vertices) {
      graph_json += print_vertex(vertex, graph) + ",";
    }

    graph_json.pop_back();
  }

  graph_json += "],\"edges\":[";

  const auto& edges = graph.edges();

  if (!edges.empty()) {
    for (const auto& [edge_id, edge] : edges) {
      graph_json += print_edge(edge) + ",";
    }

    graph_json.pop_back();
  }

  graph_json += "]}";

  return graph_json;
}
