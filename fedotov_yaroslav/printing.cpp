#include "printing.hpp"
#include <sstream>

namespace printing {
namespace json {

std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::ostringstream result;
  bool is_first = true;
  const auto& edges_ids = graph.connected_edge_ids(vertex.id());
  result << "\n{ \"id\": " << vertex.id() << ", \"edges_ids\": [";
  for (const auto edge_id : edges_ids) {
    if (!is_first)
      result << ", " << edge_id;
    else {
      result << edge_id;
      is_first = false;
    }
  }
  result << "] }";
  return result.str();
}

std::string print_edge(const Graph::Edge& edge) {
  std::ostringstream result;
  result << "\n{\"id\": " << edge.id() << ", \"vertex_ids\": ["
         << edge.from_vertex_id() << ", " << edge.to_vertex_id() << "]}";
  return result.str();
}

std::string print_graph(const Graph& graph) {
  std::string result = "{\"vertices\": [";
  bool is_first = true;
  for (const auto& [_, vertex] : graph.vertices()) {
    if (!is_first) {
      result.append(", ");
    } else {
      is_first = false;
    }
    result.append(print_vertex(vertex, graph));
  }

  result.append("],\n\"edges\": [");
  is_first = true;
  for (const auto& [_, edge] : graph.edges()) {
    if (!is_first) {
      result.append(", ");
    } else {
      is_first = false;
    }
    result.append(print_edge(edge));
  }
  result.append("]}\n");
  return result;
}
};  // namespace json

};  // namespace printing
