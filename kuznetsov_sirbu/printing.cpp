#include "printing.hpp"
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include "graph_json_printing.hpp"
#include "graph_traverser.hpp"

namespace uni_course_cpp {
namespace printing {

static constexpr int kUseColors = 4;
constexpr std::array<Color, kUseColors> kColors = {Color::Grey, Color::Green,
                                                   Color::Yellow, Color::Red};

std::string print_graph(const IGraph& graph) {
  std::ostringstream result;
  result << "{\n\tdepth: " << graph.depth() << ",\n";
  result << "\tvertices: "
         << "{amount: " << graph.get_vertices().size() << ", "
         << "distribution: [";
  for (Depth depth = 1; depth <= graph.depth(); ++depth) {
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

std::string print_path(const GraphTraverser::GraphPath& path) {
  std::string res;
  res = "{vertices: [";
  for (const auto& vertex_id : path.vertex_ids()) {
    res += std::to_string(vertex_id);
    res += ", ";
  }
  if (path.vertex_ids().size()) {
    res.pop_back();
    res.pop_back();
  }
  res += "], ";

  res += "\"edges\": [ ";
  for (const auto& edge_id : path.edge_ids()) {
    res += std::to_string(edge_id);
    res += ", ";
  }
  if (path.edge_ids().size() > 0) {
    res.pop_back();
    res.pop_back();
  }
  res += " ], distance: ";
  res += std::to_string(path.distance());
  res += "}";

  return res;
}

}  // namespace printing
}  // namespace uni_course_cpp
