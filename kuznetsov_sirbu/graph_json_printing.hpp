#pragma once

#include <string>
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {
namespace printing {
namespace json {

std::string print_vertex(const IVertex& vertex, const IGraph& graph);
std::string print_edge_color(EdgeColor edge_color);
std::string print_edge(const IEdge& edge);
std::string print_graph(const IGraph& graph);

}  // namespace json
}  // namespace printing
}  // namespace uni_course_cpp
