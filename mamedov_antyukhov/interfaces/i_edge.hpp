#pragma once

#include "i_vertex.hpp"

namespace uni_course_cpp {
using EdgeId = int;
enum class EdgeColor { Grey, Green, Yellow, Red };
class IEdge {
 public:
  virtual ~IEdge(){};
  virtual EdgeId id() const = 0;
  virtual VertexId get_first_vertex_id() const = 0;
  virtual VertexId get_second_vertex_id() const = 0;
  virtual EdgeColor color() const = 0;
};
}  // namespace uni_course_cpp
