#include "printing_json.hpp"
#include <sstream>
#include "printing.hpp"

namespace uni_course_cpp {
namespace json {

std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::ostringstream result;
  bool is_first = true;
  const auto& edges_ids = graph.connected_edge_ids(vertex.id());
  result << "\n{ \"id\": " << vertex.id()
         << ", \"depth\": " << graph.get_vertex_depth(vertex.id())
         << ", \"edge_ids\": [";
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
  result << "\n{\"id\": " << edge.id() << ", \"color\": \""
         << printing::print_edge_color(edge.color()) << "\", \"vertex_ids\": ["
         << edge.from_vertex_id() << ", " << edge.to_vertex_id() << "]}";
  return result.str();
}

std::string print_graph(const Graph& graph) {
  std::ostringstream result;
  result << "{\"depth\": " << graph.depth() << ",\n \"vertices\": [";
  bool is_first = true;
  for (const auto& [_, vertex] : graph.vertices()) {
    if (!is_first) {
      result << ", ";
    } else {
      is_first = false;
    }
    result << print_vertex(vertex, graph);
  }

  result << "],\n\"edges\": [";
  is_first = true;
  for (const auto& [_, edge] : graph.edges()) {
    if (!is_first) {
      result << ", ";
    } else {
      is_first = false;
    }
    result << print_edge(edge);
  }
  result << "]}\n";
  return result.str();
}
}  // namespace json
}  // namespace uni_course_cpp
