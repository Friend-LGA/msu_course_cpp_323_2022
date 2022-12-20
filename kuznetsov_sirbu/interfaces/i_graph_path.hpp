#pragma once

#include "i_edge.hpp"
#include "i_graph.hpp"
#include "i_vertex.hpp"

namespace uni_course_cpp {

using PathDistance = int;

struct IGraphPath {
  virtual ~IGraphPath(){};
  virtual PathDistance distance() const = 0;
  virtual const std::vector<VertexId>& vertex_ids() const = 0;
  virtual const std::vector<EdgeId>& edge_ids() const = 0;
};

}  // namespace uni_course_cpp
