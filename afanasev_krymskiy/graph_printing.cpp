#include "graph_printing.hpp"
#include <array>
#include <string>
#include "graph.hpp"

namespace {
constexpr std::array<uni_course_cpp::Graph::Edge::Color, 4> kColors = {
    uni_course_cpp::Graph::Edge::Color::Grey,
    uni_course_cpp::Graph::Edge::Color::Green,
    uni_course_cpp::Graph::Edge::Color::Yellow,
    uni_course_cpp::Graph::Edge::Color::Red};
}

namespace uni_course_cpp {
std::string printing::print_edge_color(Graph::Edge::Color color) {
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
std::string printing::print_graph(const Graph& graph) {
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
