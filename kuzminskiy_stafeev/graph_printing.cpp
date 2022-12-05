#include "graph_printing.hpp"
#include <array>

namespace uni_course_cpp {

namespace {

static constexpr std::array<Graph::Edge::Color, 4> edge_colors = {
    Graph::Edge::Color::Grey, Graph::Edge::Color::Green,
    Graph::Edge::Color::Yellow, Graph::Edge::Color::Red};

}

std::string printing::print_edge_color(const Graph::Edge::Color& color) {
  switch (color) {
    case Graph::Edge::Color::Grey:
      return "grey";
    case Graph::Edge::Color::Yellow:
      return "yellow";
    case Graph::Edge::Color::Red:
      return "red";
    case Graph::Edge::Color::Green:
      return "green";
    default:
      return "No color";
  }
}

std::string printing::print_graph(const Graph& graph) {
  const auto depth = graph.get_graph_depth();

  const auto vertices = graph.vertices().size();
  const auto edges = graph.edges().size();

  std::string depths_distribution = "";

  for (int i = 1; i < depth + 1; i++) {
    depths_distribution += std::to_string(graph.vertices_of_depth(i).size());

    if (i != depth) {
      depths_distribution += ", ";
    }
  }

  std::string edges_distribution = "";
  for (int i = 0; i < edge_colors.size(); i++) {
    edges_distribution +=
        print_edge_color(edge_colors[i]) + ": " +
        std::to_string(graph.edges_ids_of_color(edge_colors[i]).size());
    if (i != edge_colors.size() - 1) {
      edges_distribution += ", ";
    }
  }

  return "  depth: " + std::to_string(depth) + ",\n" +
         "  vertices: {amount: " + std::to_string(vertices) +
         ", distribution: " + "[" + depths_distribution + "]},\n" +
         "  edges: {amount: " + std::to_string(edges) +
         ", distribution: " + "{" + edges_distribution + "}}";
}

}  // namespace uni_course_cpp
