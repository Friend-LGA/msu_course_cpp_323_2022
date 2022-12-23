#include "graph_printing.hpp"
#include <array>
#include <iostream>
#include <stdexcept>

namespace uni_course_cpp {

namespace {

static constexpr std::array<EdgeColor, 4> edge_colors = {
    EdgeColor::Grey, EdgeColor::Green, EdgeColor::Yellow, EdgeColor::Red};

}

std::string printing::print_edge_color(const EdgeColor& color) {
  switch (color) {
    case EdgeColor::Grey:
      return "grey";
    case EdgeColor::Yellow:
      return "yellow";
    case EdgeColor::Red:
      return "red";
    case EdgeColor::Green:
      return "green";
    default:
      throw std::runtime_error("Failed to determine color");
  }
}

std::string printing::print_graph(const IGraph& graph) {
  const auto depth = graph.get_graph_depth();

  const auto vertices = graph.get_vertexes().size();
  const auto edges = graph.get_edges().size();

  std::string depths_distribution = "";

  for (GraphDepth i = 1; i < depth + 1; i++) {
    depths_distribution += std::to_string(graph.vertexes_of_depth(i).size());

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
