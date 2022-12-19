#include <array>
#include <string>

#include "graph.hpp"
#include "graph_printing.hpp"
#include "interfaces/i_graph.hpp"

namespace {
constexpr std::array<uni_course_cpp::EdgeColor, 4> kColors = {
    uni_course_cpp::EdgeColor::Grey, uni_course_cpp::EdgeColor::Green,
    uni_course_cpp::EdgeColor::Yellow, uni_course_cpp::EdgeColor::Red};
}

namespace uni_course_cpp {
std::string printing::print_edge_color(EdgeColor color) {
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
std::string printing::print_graph(const IGraph& graph) {
  std::string result = "{\n";

  result += "  depth: " + std::to_string(graph.depth()) + ",\n";

  result += "  vertices: {amount: " + std::to_string(graph.vertices().size()) +
            ", distribution: [";
  for (auto depth = kDefaultDepth; depth <= graph.depth(); ++depth) {
    result += std::to_string(graph.vertices_at_depth(depth).size());
    if (depth != graph.depth()) {
      result += ", ";
    }
  }
  result += "]},\n";

  result += "  edges: {amount: " + std::to_string(graph.edges().size()) +
            ", distribution: {";
  for (const auto color : kColors) {
    result += printing::print_edge_color(color) + ": " +
              std::to_string(graph.color_edge_ids(color).size()) + ", ";
  }
  if (!kColors.empty()) {
    result.pop_back();
    result.pop_back();
  }
  result += "}}\n";

  result += "}\n";
  return result;
}
}  // namespace uni_course_cpp
