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

  virtual GraphDepth get_depth() const = 0;

  virtual const std::vector<VertexId>& get_depth_vertex_ids(
      GraphDepth) const = 0;

  virtual const std::vector<EdgeId>& get_connected_edge_ids(VertexId) const = 0;

  virtual bool is_vertices_connected(VertexId, VertexId) const = 0;

  virtual GraphDepth get_vertex_depth(VertexId) const = 0;

  virtual const std::unordered_map<VertexId, std::unique_ptr<IVertex>>&
  get_vertices() const = 0;

  virtual const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& get_edges()
      const = 0;
};
}  // namespace uni_course_cpp
