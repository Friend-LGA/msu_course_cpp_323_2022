#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "i_edge.hpp"
#include "i_vertex.hpp"

namespace uni_course_cpp {
using Depth = int;
class IGraph {
 public:
  virtual ~IGraph(){};

  virtual VertexId add_vertex() = 0;
  virtual EdgeId add_edge(VertexId from_vertex_id, VertexId to_vertex_id) = 0;
  virtual bool has_vertex(VertexId vertex_id) const = 0;
  virtual const std::vector<std::unique_ptr<IVertex>>& get_vertices() const = 0;
  virtual const std::vector<std::unique_ptr<IEdge>>& get_edges() const = 0;
  virtual const std::vector<EdgeId>& connected_edges_ids(
      VertexId vertex_id) const = 0;
  virtual void set_vertex_depth(VertexId vertex_id, Depth depth) = 0;
  virtual bool is_connected(VertexId from_vertex_id,
                            VertexId to_vertex_id) const = 0;
  virtual Color get_edge_color(VertexId from_vertex_id,
                               VertexId to_vertex_id) const = 0;
  virtual Depth vertex_depth(VertexId vertex_id) const = 0;
  virtual const std::vector<VertexId>& get_vertices_with_depth(
      Depth depth) const = 0;
  virtual const std::vector<EdgeId>& get_edge_ids_with_color(
      Color color) const = 0;
  virtual Depth depth() const = 0;
  virtual std::pair<VertexId, VertexId> get_vertex_ids_from_edge_id(
      EdgeId edge_id) const = 0;
};
}  // namespace uni_course_cpp
