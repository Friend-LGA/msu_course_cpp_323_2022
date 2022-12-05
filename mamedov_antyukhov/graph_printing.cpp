#include "graph_printing.hpp"
#include <array>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include "graph.hpp"

namespace {
static constexpr int kColorsAmount = 4;
constexpr std::array<uni_course_cpp::Graph::Edge::Color, kColorsAmount>
    kColors = {uni_course_cpp::Graph::Edge::Color::Grey,
               uni_course_cpp::Graph::Edge::Color::Green,
               uni_course_cpp::Graph::Edge::Color::Yellow,
               uni_course_cpp::Graph::Edge::Color::Red};
}  // namespace

namespace uni_course_cpp {
namespace printing {
std::string print_edge_color(Graph::Edge::Color color) {
  switch (color) {
    case Graph::Edge::Color::Grey:
      return "grey";
    case Graph::Edge::Color::Green:
      return "green";
    case Graph::Edge::Color::Red:
      return "red";
    case Graph::Edge::Color::Yellow:
      return "yellow";
    default:
      throw std::runtime_error("Can't recognize edge color");
      return "unknown color";
  }
}

std::string print_graph(const Graph& graph) {
  std::stringstream data_graph;
  const auto& vertices_depth = graph.get_depth_levels();
  const auto& edges = graph.get_edges();
  data_graph << "{\n\tdepth:" << graph.depth() << ",\n";
  data_graph << "\tvertices: { amount: " << vertices_depth.size()
             << ", distribution: [";

  bool is_first_iteration = true;
  for (int i = 0; i < vertices_depth.size(); ++i) {
    if (!is_first_iteration)
      data_graph << ", ";
    data_graph << vertices_depth[i].size();
    is_first_iteration = false;
  }
  data_graph << "]},\n";

  data_graph << "\tedges: {amount: " << edges.size() << ", distribution: {";

  is_first_iteration = true;
  for (int i = 0; i < kColorsAmount; ++i) {
    if (!is_first_iteration)
      data_graph << ", ";
    data_graph << print_edge_color(kColors[i]) << " : "
               << graph.get_colored_edge_ids(kColors[i]).size();
    is_first_iteration = false;
  }
  data_graph << "}}\n}";

  return data_graph.str();
}

}  // namespace printing
}  // namespace uni_course_cpp
