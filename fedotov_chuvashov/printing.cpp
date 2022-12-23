#include "printing.hpp"
#include <sstream>

namespace uni_course_cpp {
namespace printing {
std::string print_edge_color(EdgeColor color) {
  switch (color) {
    case EdgeColor::Grey:
      return "grey";
    case EdgeColor::Yellow:
      return "yellow";
    case EdgeColor::Red:
      return "red";
    case EdgeColor::Green:
      return "green";
  }
}

std::string print_graph(const IGraph& graph) {
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
         << graph.get_colored_edge_ids(EdgeColor::Grey).size()
         << ", green: " << graph.get_colored_edge_ids(EdgeColor::Green).size()
         << ", yellow: " << graph.get_colored_edge_ids(EdgeColor::Yellow).size()
         << ", red: " << graph.get_colored_edge_ids(EdgeColor::Red).size()
         << "}}\n}";
  return result.str();
}
};  // namespace printing
}  // namespace uni_course_cpp
