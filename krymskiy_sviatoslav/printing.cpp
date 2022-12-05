#include <fstream>
#include <unordered_map>
#include <vector>

#include "graph_generator.hpp"
#include "printing.hpp"

std::string printing::json::print_graph(const Graph& graph) {
  const auto& vertices = graph.get_vertices();
  const auto& edges = graph.get_edges();
  std::string graph_string = "{\n\t\"vertices\": [\n";
  bool first = true;

  for (const auto& [vertex_id, vertex] : vertices) {
    if (!first) {
      graph_string += ",\n";
    }
    first = false;
    graph_string += "\t\t" + print_vertex(vertex, graph);
  }

  graph_string += "\n\t],\n\t\"edges\": [\n";
  first = true;
  for (const auto& [edge_id, edge] : edges) {
    if (!first) {
      graph_string += ",\n";
    }
    first = false;
    graph_string += "\t\t" + print_edge(edge);
  }
  graph_string += "\n\t]\n}\n";
  return graph_string;
}

std::string printing::json::print_vertex(const Graph::Vertex& vertex,
                                         const Graph& graph) {
  const auto vertex_id = vertex.id();
  std::string vertex_string =
      "{ \"id\": " + std::to_string(vertex_id) + ", \"edge_ids\": [";
  bool first = true;
  const auto& vertex_edge_ids = graph.get_vertex_edge_ids(vertex_id);
  for (const auto edge_id : vertex_edge_ids) {
    if (!first) {
      vertex_string += ", ";
    }
    first = false;
    vertex_string += std::to_string(edge_id);
  }
  vertex_string += "] }";
  return vertex_string;
}

std::string printing::json::print_edge(const Graph::Edge& edge) {
  return "{ \"id\": " + std::to_string(edge.id()) + ", \"vertex_ids\": [" +
         std::to_string(edge.to_vertex_id()) + ", " +
         std::to_string(edge.from_vertex_id()) + "] }";
}
