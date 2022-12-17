#include "graph_json_printing.hpp"
#include "graph_printing.hpp"
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {
namespace printing {
namespace json {
std::string print_vertex(const IVertex& vertex, const IGraph& graph) {
  std::string vertex_json =
      "{\"id\":" + std::to_string(vertex.id()) + ",\"edge_ids\":[";
  const auto& connected_edge_ids = graph.get_connected_edge_ids(vertex.id());

  if (connected_edge_ids.size() != 0) {
    for (const auto edge_id : connected_edge_ids) {
      vertex_json += std::to_string(edge_id) + ",";
    }

    vertex_json.pop_back();
  }

  vertex_json +=
      "],\"depth\":" + std::to_string(graph.get_vertex_depth(vertex.id()));

  vertex_json += "}";

  return vertex_json;
}

std::string print_edge(const IEdge& edge) {
  std::string edge_json =
      "{\"id\":" + std::to_string(edge.id()) + ",\"vertex_ids\":[";

  edge_json += std::to_string(edge.from_vertex_id()) + "," +
               std::to_string(edge.to_vertex_id());
  edge_json += "],\"color\":\"" + print_edge_color(edge.color()) + "\"";
  edge_json += "}";

  return edge_json;
}

std::string print_graph(const IGraph& graph) {
  const auto& vertices = graph.get_vertices();
  const auto& edges = graph.get_edges();

  std::string graph_json =
      "{\n\t\"depth\":" + std::to_string(graph.get_depth()) + ",";

  graph_json += "\n\t\"vertices\": [\n";
  if (vertices.size() != 0) {
    for (const auto& [vertex_id, vertex] : vertices) {
      graph_json += "\t\t" + print_vertex(*vertex, graph) + ",\n";
    }
    graph_json.pop_back();
    graph_json.pop_back();
  }

  graph_json += "\n\t],\n\t\"edges\":[\n";

  if (edges.size() != 0) {
    for (const auto& [edge_id, edge] : edges) {
      graph_json += "\t\t" + print_edge(*edge) + ",\n";
    }
    graph_json.pop_back();
    graph_json.pop_back();
  }

  graph_json += "\n\t]\n}\n";

  return graph_json;
}
}  // namespace json
}  // namespace printing
}  // namespace uni_course_cpp
