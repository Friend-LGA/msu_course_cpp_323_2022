#pragma once
#include <string>
#include "graph.hpp"

namespace uni_course_cpp {
namespace json {

std::string print_vertex(const IVertex& vertex, const IGraph& graph);

std::string print_edge(const IEdge& edge);

std::string print_graph(const IGraph& graph);
}  // namespace json
}  // namespace uni_course_cpp
