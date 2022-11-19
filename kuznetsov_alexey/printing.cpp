#include "printing.hpp"

std::string uni_course_cpp::printing::print_graph(const Graph& graph) {
  std::ostringstream result;
  result << "\tdepth: " << graph.depth() << ",\n";
  result << "\tvertices: "
         << "{amount: " << graph.get_vertices().size() << ", "
         << "distribution: [";
  for (Graph::Depth depth = 1; depth <= graph.depth(); ++depth) {
    result << graph.get_vertices_with_depth(depth).size();
    if (depth < graph.depth()) {
      result << ", ";
    }
  }
  result << "]},\n";

  result << "]},\n\tedges: {amount: " << graph.get_edges().size();
  result << ", distribution: {grey: "
         << graph.get_edges_with_color(Graph::Edge::Color::Grey).size();
  result << ", green: "
         << graph.get_edges_with_color(Graph::Edge::Color::Green).size();
  result << ", yellow: "
         << graph.get_edges_with_color(Graph::Edge::Color::Yellow).size();
  result << ", red: "
         << graph.get_edges_with_color(Graph::Edge::Color::Red).size()
         << "}}\n}";

  return result.str();
}
