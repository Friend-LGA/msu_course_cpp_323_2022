#pragma once

#include "graph.hpp"
#include "interfaces/i_edge.hpp"

namespace uni_course_cpp {
namespace printing {
std::string print_graph(const IGraph& graph);
std::string print_edge_color(EdgeColor color);
}  // namespace printing
}  // namespace uni_course_cpp
