#pragma once
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>
#include "i_edge.hpp"
#include "i_vertex.hpp"

namespace uni_course_cpp {

using GraphDepth = int;

class IGraph {
 public:
  virtual ~IGraph(){};

  virtual bool is_connected(VertexId, VertexId) const = 0;
  virtual void add_edge(VertexId, VertexId) = 0;
  virtual VertexId add_vertex() = 0;
  virtual GraphDepth get_graph_depth() const = 0;
  virtual GraphDepth get_vertex_depth(VertexId id) const = 0;
  virtual const std::unordered_map<VertexId, std::unique_ptr<IVertex>>&
  get_vertexes() const = 0;
  virtual const std::vector<VertexId>& vertexes_of_depth(GraphDepth) const = 0;
  virtual const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& get_edges()
      const = 0;
  virtual const std::set<EdgeId>& connected_edge_ids(VertexId) const = 0;
  virtual const std::vector<EdgeId>& edges_ids_of_color(
      EdgeColor color) const = 0;
  virtual bool has_vertex(VertexId id) const = 0;
  virtual bool has_edge(EdgeId id) const = 0;
};

}  // namespace uni_course_cpp
