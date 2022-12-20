#include "printing.hpp"
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include "graph_json_printing.hpp"

namespace uni_course_cpp {
namespace printing {

static constexpr int kUseColors = 4;
constexpr std::array<Graph::Edge::Color, kUseColors> kColors = {
    Graph::Edge::Color::Grey, Graph::Edge::Color::Green,
    Graph::Edge::Color::Yellow, Graph::Edge::Color::Red};

std::string print_graph(const Graph& graph) {
  std::ostringstream result;
  result << "{\n\tdepth: " << graph.depth() << ",\n";
  result << "\tvertices: "
         << "{amount: " << graph.get_vertices().size() << ", "
         << "distribution: [";
  for (Graph::Depth depth = 1; depth <= graph.depth(); ++depth) {
    result << graph.get_vertices_with_depth(depth).size();
    if (depth < graph.depth()) {
      result << ", ";
    }
  }

  result << "]},\n\tedges: {amount: " << graph.get_edges().size();

  result << ", distribution: {";
  for (const auto color : kColors) {
    result << json::print_edge_color(color) << ": "
           << graph.get_edge_ids_with_color(Graph::Edge::Color::Grey).size()
           << ", ";
  }
  auto graph_string = result.str();
  graph_string.pop_back();
  graph_string.pop_back();
  graph_string += "}}\n}";
  return graph_string;
}

}  // namespace printing
}  // namespace uni_course_cpp
