#pragma once
#include <string>
#include "graph.hpp"

namespace uni_course_cpp {

namespace printing {
std::string print_edge_color(const Graph::Edge::Color& color);
std::string print_graph(const Graph& graph);
}  // namespace printing

}  // namespace uni_course_cpp
