#pragma once
#include <string>
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {

namespace printing {
std::string print_edge_color(const EdgeColor& color);
std::string print_graph(const IGraph& graph);
}  // namespace printing

}  // namespace uni_course_cpp
