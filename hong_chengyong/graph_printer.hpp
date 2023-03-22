#pragma once

#include <string>
#include "graph.hpp"

namespace uni_course_cpp {
class GraphPrinter {
 public:
  explicit GraphPrinter(const Graph& graph) : graph_(graph) {}
  std::string print() const;
  static std::string colorToString(const Edge::Colors& color);
  std::string printVertex(const VertexId& id) const;
  std::string printEdge(const Edge& edge) const;

 private:
  const Graph& graph_;
};
}  // namespace uni_course_cpp
