#pragma once

#include "graph.hpp"

namespace uni_course_cpp {
namespace printing {
namespace json {
std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph);

std::string print_edge(const Graph::Edge& edge);

std::string print_graph(const Graph& graph);
}  // namespace json
}  // namespace printing
}  // namespace uni_course_cpp
