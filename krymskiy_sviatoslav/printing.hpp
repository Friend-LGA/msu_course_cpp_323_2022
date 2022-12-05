#pragma once

namespace printing {
namespace json {
std::string print_graph(const Graph& graph);
std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph);
std::string print_edge(const Graph::Edge& edge);
}  // namespace json
}  // namespace printing
