#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "i_edge.hpp"
#include "i_vertex.hpp"

namespace uni_course_cpp {
using GraphDepth = int;
class IGraph {
 public:
  virtual ~IGraph(){};
  virtual VertexId add_vertex() = 0;
  virtual EdgeId add_edge(VertexId, VertexId) = 0;
  virtual const std::vector<EdgeId>& connected_edge_ids(VertexId id) const = 0;
  virtual const std::unordered_map<VertexId, std::unique_ptr<IVertex>>&
  vertices() const = 0;
  virtual const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& edges()
      const = 0;
  virtual GraphDepth depth() const = 0;
  virtual GraphDepth vertex_depth(VertexId vertex_id) const = 0;
  virtual const std::vector<VertexId>& vertices_at_depth(
      GraphDepth depth) const = 0;
  virtual bool has_edge(VertexId, VertexId) const = 0;
  virtual const std::vector<EdgeId>& color_edge_ids(EdgeColor color) const = 0;
};
}  // namespace uni_course_cpp
