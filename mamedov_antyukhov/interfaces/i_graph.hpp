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

  virtual EdgeId add_edge(VertexId first_vertex_id,
                          VertexId second_vertex_id) = 0;

  virtual bool has_edge(VertexId first_vertex_id,
                        VertexId second_vertex_id) const = 0;
  virtual GraphDepth depth() const = 0;

  virtual const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& get_edges()
      const = 0;

  virtual const std::unordered_map<VertexId, std::unique_ptr<IVertex>>&
  get_vertices() const = 0;

  virtual const std::vector<EdgeId>& get_connected_edge_ids(
      VertexId vertex_id) const = 0;

  virtual const std::vector<std::vector<VertexId>>& get_depth_levels()
      const = 0;
  virtual GraphDepth get_vertex_depth(VertexId vertex_id) const = 0;

  virtual const std::vector<VertexId>& get_vertex_ids_at_depth(
      GraphDepth depth) const = 0;

  virtual const std::vector<EdgeId>& get_colored_edge_ids(
      EdgeColor color) const = 0;

  virtual VertexId get_new_vertex_id() = 0;

  virtual EdgeId get_new_edge_id() = 0;
};
}  // namespace uni_course_cpp