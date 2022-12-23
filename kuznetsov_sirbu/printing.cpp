#include "printing.hpp"
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include "graph_json_printing.hpp"
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {
namespace printing {

static constexpr int kUseColors = 4;
constexpr std::array<EdgeColor, kUseColors> kColors = {
    EdgeColor::Grey, EdgeColor::Green, EdgeColor::Yellow, EdgeColor::Red};

std::string print_graph(const IGraph& graph) {
  std::ostringstream result;
  result << "{\n\tdepth: " << graph.depth() << ",\n";
  result << "\tvertices: "
         << "{amount: " << graph.get_vertices().size() << ", "
         << "distribution: [";
  for (GraphDepth depth = kGraphBaseDepth; depth <= graph.depth(); ++depth) {
    result << graph.get_vertices_with_depth(depth).size();
    if (depth < graph.depth()) {
      result << ", ";
    }
  }

  result << "]},\n\tedges: {amount: " << graph.get_edges().size();

  result << ", distribution: {";
  for (const auto color : kColors) {
    result << json::print_edge_color(color) << ": "
           << graph.get_edge_ids_with_color(color).size() << ", ";
  }
  auto graph_string = result.str();
  graph_string.pop_back();
  graph_string.pop_back();
  graph_string += "}}\n}";
  return graph_string;
}

}  // namespace printing
}  // namespace uni_course_cpp
