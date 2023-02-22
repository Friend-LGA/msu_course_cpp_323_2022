#pragma once

#include "i_vertex.hpp"

namespace uni_course_cpp {
using EdgeId = int;
enum class EdgeColor { Grey, Green, Yellow, Red };
struct IEdge {
 public:
  virtual ~IEdge(){};
  virtual EdgeId id() const = 0;
  virtual VertexId from_vertex_id() const = 0;
  virtual VertexId to_vertex_id() const = 0;
  virtual EdgeColor color() const = 0;
};
}  // namespace uni_course_cpp
