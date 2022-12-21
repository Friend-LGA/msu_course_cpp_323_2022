#include "graph_printing.hpp"

namespace uni_course_cpp {
namespace printing {

std::string print_graph(const Graph& graph) {
  std::ostringstream graph_data_string;
  graph_data_string << "{\n\tdepth: " << graph.depth()
                    << ",\n\tvertices: {amount: " << graph.vertices().size()
                    << ", distribution: [";
  std::string separator = "";
  for (int i = Graph::kGraphBaseDepth; i < graph.depth(); ++i) {
    graph_data_string << separator << graph.vertices_at_depth(i).size();
    separator = ",";
  }

  graph_data_string << "]},\n\tedges: {amount: " << graph.edges().size()
                    << ", distribution: {";

  for (int color = 0; color < sizeof(Graph::Edge::Color); ++color) {
    graph_data_string
        << print_edge_color((Graph::Edge::Color)(color)) << ": "
        << graph.get_colored_edges_ids((Graph::Edge::Color)(color)).size();
    if (color != (sizeof(Graph::Edge::Color) - 1)) {
      graph_data_string << ", ";
    } else {
      graph_data_string << "}}\n}";
    }
  }
  return graph_data_string.str();
}

std::string print_edge_color(Graph::Edge::Color edge_color) {
  switch (edge_color) {
    case Graph::Edge::Color::Grey:
      return "grey";
    case Graph::Edge::Color::Green:
      return "green";
    case Graph::Edge::Color::Yellow:
      return "yellow";
    case Graph::Edge::Color::Red:
      return "red";
    default:
      throw std::runtime_error("Failed to determine color name");
  }
}
}  // namespace printing
}  // namespace uni_course_cpp
