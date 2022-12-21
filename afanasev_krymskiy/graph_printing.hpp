#pragma once

#include <string>

#include "graph.hpp"
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {
namespace printing {
std::string print_edge_color(EdgeColor);
std::string print_graph(const IGraph&);
}  // namespace printing
}  // namespace uni_course_cpp
