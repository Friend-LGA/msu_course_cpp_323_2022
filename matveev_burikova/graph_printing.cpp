#include "graph_printing.hpp"
#include "interfaces/i_edge.hpp"

#include <array>
#include <string>

namespace {
using Graph = uni_course_cpp::Graph;
constexpr std::array<uni_course_cpp::EdgeColor, 4> kColorArray = {
    uni_course_cpp::EdgeColor::Grey, uni_course_cpp::EdgeColor::Green,
    uni_course_cpp::EdgeColor::Yellow, uni_course_cpp::EdgeColor::Red};
}  // namespace

namespace uni_course_cpp {
namespace printing {
std::string print_graph(const IGraph& graph) {
  std::string graph_info_string;
  graph_info_string =
      "{\n  depth: " + std::to_string(graph.depth()) +
      ",\n  vertices: {amount: " + std::to_string(graph.get_vertices().size()) +
      ", distribution: [";
  for (int depth = 1; depth <= graph.depth(); ++depth) {
    graph_info_string +=
        std::to_string(graph.get_vertex_ids_on_depth(depth).size()) + ", ";
  }
  graph_info_string.erase(graph_info_string.length() - 2,
                          graph_info_string.length());
  graph_info_string +=
      "]},\n  edges: {amount: " + std::to_string(graph.get_edges().size()) +
      ", distribution: {";
  std::array<int, kColorArray.size()> color_distribution_counts = {0};
  for (const auto& [edge_id, edge] : graph.get_edges())
    color_distribution_counts[(int)edge->color()]++;
  for (int color_number = 0; color_number < sizeof(EdgeColor); color_number++) {
    graph_info_string +=
        printing::print_edge_color((EdgeColor)color_number) + ": " +
        std::to_string(color_distribution_counts[color_number]) + ", ";
  }
  graph_info_string.erase(graph_info_string.length() - 2,
                          graph_info_string.length());
  return graph_info_string + "}}\n}";
}

std::string print_edge_color(EdgeColor color) {
  switch (color) {
    case EdgeColor::Green:
      return "green";
    case EdgeColor::Yellow:
      return "yellow";
    case EdgeColor::Red:
      return "red";
    default:
      return "grey";
  }
}
}  // namespace printing
}  // namespace uni_course_cpp
