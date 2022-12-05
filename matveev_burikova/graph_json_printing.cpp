#include "graph_json_printing.hpp"
#include "graph_printing.hpp"

namespace uni_course_cpp {
namespace printing {
namespace json {

std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::string edges_ids_string = "[";
  for (const Graph::EdgeId edge_id :
       graph.get_connected_edge_ids(vertex.id())) {
    edges_ids_string += std::to_string(edge_id) + ',';
  }
  edges_ids_string.erase(edges_ids_string.length() - 1,
                         edges_ids_string.length());
  return "\"id\":" + std::to_string(vertex.id()) +
         ",\"edge_ids\":" + edges_ids_string + "]" +
         ",\"depth\":" + std::to_string(vertex.depth);
}

std::string print_edge(const Graph::Edge& edge) {
  return "\"id\":" + std::to_string(edge.id()) + ",\"vertex_ids\":" + "[" +
         std::to_string(edge.from_vertex_id()) + "," +
         std::to_string(edge.to_vertex_id()) + "],\"color\":\"" +
         print_edge_color(edge.color()) + "\"";
}

std::string print_graph(const Graph& graph) {
  std::string graph_string = "{\n  \"vertices\": [\n";
  for (const auto& [vertex_id, vertex] : graph.get_vertices()) {
    graph_string += "    {" + print_vertex(vertex, graph) + "},\n";
  }
  graph_string.erase(graph_string.length() - 2, graph_string.length());
  graph_string += "\n  ],\n  \"edges\": [\n";
  for (const auto& [edge_id, edge] : graph.get_edges()) {
    graph_string += "    {" + print_edge(edge) + "},\n";
  }
  graph_string.erase(graph_string.length() - 2, graph_string.length());
  return graph_string + "\n  ]\n}\n";
}
}  // namespace json
}  // namespace printing
}  // namespace uni_course_cpp
