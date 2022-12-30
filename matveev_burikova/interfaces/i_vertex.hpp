#pragma once

namespace uni_course_cpp {

using VertexId = int;
using GraphDepth = int;

struct IVertex {
 public:
  virtual ~IVertex(){};

  virtual VertexId id() const = 0;
  virtual GraphDepth get_depth() const = 0;
  virtual void set_depth(GraphDepth new_depth) = 0;
};

}  // namespace uni_course_cpp
