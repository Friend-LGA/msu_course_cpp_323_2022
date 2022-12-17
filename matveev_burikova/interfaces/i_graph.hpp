#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>
#include "i_edge.hpp"
#include "i_vertex.hpp"

namespace uni_course_cpp {

// using GraphDepth = int;

class IGraph {
 public:
  virtual ~IGraph(){};

  virtual VertexId add_vertex() = 0;
  virtual EdgeColor get_new_edge_color(VertexId, VertexId) = 0;
  virtual void add_edge(VertexId, VertexId) = 0;
  virtual GraphDepth depth() const = 0;
  virtual GraphDepth get_vertex_depth(VertexId) const = 0;
  virtual const std::unordered_map<VertexId, IVertex*>& get_vertices()
      const = 0;
  virtual const std::unordered_map<EdgeId, IEdge*>& get_edges() const = 0;
  virtual const std::vector<EdgeId>& get_connected_edge_ids(VertexId) const = 0;
  virtual const std::vector<VertexId>& get_vertex_ids_on_depth(
      GraphDepth) const = 0;
  virtual bool has_vertex(VertexId) const = 0;
  virtual bool has_edge(VertexId, VertexId) const = 0;
};

}  // namespace uni_course_cpp
