#include "printing.hpp"
#include <sstream>

namespace uni_course_cpp {
namespace printing {
std::string print_edge_color(Graph::Edge::Color color) {
  switch (color) {
    case Graph::Edge::Color::Grey:
      return "grey";
    case Graph::Edge::Color::Yellow:
      return "yellow";
    case Graph::Edge::Color::Red:
      return "red";
    case Graph::Edge::Color::Green:
      return "green";
  }
}

std::string print_graph(const Graph& graph) {
  std::ostringstream result;
  result << "{\n\tdepth: " << graph.depth()
         << ",\n\tvertices: {amount: " << graph.vertices().size()
         << ", distribution: [";
  for (int i = kGraphBaseDepth; i <= graph.depth(); ++i) {
    result << graph.vertices_at_depth(i).size();
    if (i != graph.depth())
      result << ", ";
  }
  result << "]},\n\tedges: {amount: " << graph.edges().size()
         << ", distribution: {grey: "
         << graph.get_colored_edge_ids(Graph::Edge::Color::Grey).size()
         << ", green: "
         << graph.get_colored_edge_ids(Graph::Edge::Color::Green).size()
         << ", yellow: "
         << graph.get_colored_edge_ids(Graph::Edge::Color::Yellow).size()
         << ", red: "
         << graph.get_colored_edge_ids(Graph::Edge::Color::Red).size()
         << "}}\n}";
  return result.str();
}
};  // namespace printing
}  // namespace uni_course_cpp
