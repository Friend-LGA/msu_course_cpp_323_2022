#pragma once

#include <string>
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {
namespace printing {
std::string print_depth_info(GraphDepth depth);
std::string print_edges_info(const IGraph& graph);
std::string print_edge_color(EdgeColor color);
std::string print_vertices_info(const IGraph& graph);
std::string print_graph(const IGraph& graph);
}  // namespace printing
}  // namespace uni_course_cpp
