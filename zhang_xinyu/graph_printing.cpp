#include "graph_printing.hpp"

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
