#pragma once
#include <string>
#include "graph.hpp"

namespace uni_course_cpp {
namespace printing {
std::string print_edge_color(Graph::Edge::Color);
std::string print_graph(const Graph&);
}  // namespace printing
}  // namespace uni_course_cpp
