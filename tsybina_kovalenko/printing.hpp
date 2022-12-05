#pragma once

#include <sstream>

#include "graph.hpp"

namespace printing {

std::string print_edge_color(Graph::Edge::Color edge_color) {
  switch (edge_color) {
    case Graph::Edge::Color::Grey:
      return "grey";
    case Graph::Edge::Color::Green:
      return "green";
    case Graph::Edge::Color::Yellow:
      return "yellow";
    case Graph::Edge::Color::Red:
      return "red";
    default:
      throw std::runtime_error("Failed to determine color name");
  }
}

namespace json {

// return json like {"id":0,"edge_ids":[0,1,2],"depth":1}
std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::ostringstream stream;
  stream << "{";
  stream << R"("id":)" << vertex.id() << ",";
  stream << R"("edge_ids":[)";
  std::string separator = "";

  for (auto edge_id : graph.connections_of(vertex.id())) {
    stream << separator << edge_id;
    separator = ",";
  }
  stream << "],";

  stream << R"("depth":)" << graph.depth_of(vertex.id());

  stream << "}";

  return stream.str();
}

// return json like {"id":0,"vertex_ids":[0,1],"color":"grey"}
std::string print_edge(const Graph::Edge& edge) {
  std::ostringstream stream;
  stream << "{";
  stream << R"("id":)" << edge.id() << ",";
  stream << R"("vertex_ids":[)" << edge.from_vertex_id() << ","
         << edge.to_vertex_id() << "],";
  stream << R"("color":")" << print_edge_color(edge.color()) << "\"";
  stream << "}";
  return stream.str();
}

// return json like {"depth":6,"vertices":[...],"edges":[...]}
std::string print_graph(const Graph& graph) {
  std::ostringstream stream;
  stream << "{";
  stream << R"("depth":)" << graph.depth() << ",";

  stream << R"("vertices":)"
         << "[";

  std::string separator = "";

  for (const auto& [vertex_id, vertex] : graph.vertices()) {
    stream << separator << print_vertex(vertex, graph);
    separator = ",";
  }

  stream << "],"
         << R"("edges":)"
         << "[";
  separator = "";

  for (const auto& [edge_id, edge] : graph.edges()) {
    stream << separator << print_edge(edge);
    separator = ",";
  }

  stream << "]";
  stream << "}";

  return stream.str();
}

}  // namespace json
}  // namespace printing
