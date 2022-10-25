#pragma once

#include <sstream>

#include "graph.hpp"

namespace printing {
namespace json {

// return json like {"id":0,"edge_ids":[0,1,2]}
std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::ostringstream stream;
  stream << "{";
  stream << R"("id":)" << vertex.id() << ",";
  stream << R"("edge_ids":[)";
  std::string separator = "";

  for (auto edge_id : graph.connections().at(vertex.id())) {
    stream << separator << edge_id;
    separator = ",";
  }

  stream << "]";
  stream << "}";

  return stream.str();
}

// return json like {"id":0,"vertex_ids":[0,1]}
std::string print_edge(const Graph::Edge& edge) {
  std::ostringstream stream;
  stream << "{";
  stream << R"("id":)" << edge.id() << ",";
  stream << R"("vertex_ids":[)" << edge.from_vertex_id() << ","
         << edge.to_vertex_id() << "]";
  stream << "}";
  return stream.str();
}

std::string print_graph(const Graph& graph) {
  std::ostringstream stream;
  stream << "{";
  stream << R"("vertices":)"
         << "[";

  std::string separator = "";

  for (const auto& vertex : graph.vertices()) {
    stream << separator << print_vertex(vertex, graph);
    separator = ",";
  }

  stream << "],"
         << R"("edges":)"
         << "[";
  separator = "";

  for (const auto& edge : graph.edges()) {
    stream << separator << print_edge(edge);
    separator = ",";
  }

  stream << "]";
  stream << "}";

  return stream.str();
}

}  // namespace json
}  // namespace printing