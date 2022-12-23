#pragma once
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>
#include "i_edge.hpp"
#include "i_vertex.hpp"

namespace uni_course_cpp {
using GraphDepth = int;

struct IGraph {
 public:
  virtual ~IGraph(){};

  virtual VertexId add_vertex() = 0;
  virtual EdgeId add_edge(VertexId first_vertex_id,
                          VertexId second_vertex_id) = 0;
  virtual const std::unordered_map<VertexId, std::unique_ptr<IVertex>>&
  vertices() const = 0;
  virtual const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& edges()
      const = 0;
  virtual const std::vector<EdgeId>& connected_edge_ids(
      VertexId vertex_id) const = 0;
  virtual bool is_connected(VertexId from_vertex_id,
                            VertexId to_vertex_id) const = 0;
  virtual EdgeColor get_edge_color(VertexId from_vertex_id,
                                   VertexId to_vertex_id) const = 0;
  virtual GraphDepth get_vertex_depth(VertexId vertex_id) const = 0;
  virtual void set_vertex_depth(VertexId vertex_id, GraphDepth new_depth) = 0;
  virtual const std::vector<EdgeId>& get_colored_edge_ids(
      EdgeColor color) const = 0;
  virtual GraphDepth depth() const = 0;
  virtual const std::set<VertexId>& vertices_at_depth(
      GraphDepth depth) const = 0;
  virtual std::set<VertexId> children_of_vertex(VertexId vertex_id) const = 0;
};

}  // namespace uni_course_cpp
