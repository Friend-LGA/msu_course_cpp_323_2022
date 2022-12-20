#pragma once

#include <string>
#include "graph_traverser.hpp"
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {
namespace printing {

std::string print_graph(const IGraph& graph);
std::string print_path(const GraphTraverser::GraphPath& path);

}  // namespace printing
}  // namespace uni_course_cpp
