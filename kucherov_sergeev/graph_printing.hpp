#pragma once

#include <string>
#include "graph.hpp"

namespace uni_course_cpp {
namespace printing {
std::string print_depth_info(Graph::Depth depth);
std::string print_edges_info(const Graph& graph);
std::string print_edge_color(Graph::Edge::Color color);
std::string print_vertices_info(const Graph& graph);
std::string print_graph(const Graph& graph);
}  // namespace printing
}  // namespace uni_course_cpp
