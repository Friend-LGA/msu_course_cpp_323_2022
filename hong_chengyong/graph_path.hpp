#pragma once

#include <vector>
#include "graph.hpp"

namespace uni_course_cpp {
struct GraphPath {
  using Distance = int;

  Distance distance() const;

  std::vector<VertexId> vertex_ids;
  std::vector<EdgeId> edge_ids;
};
}  // namespace uni_course_cpp
