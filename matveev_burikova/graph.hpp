#pragma once

#include "config.hpp"
#include "interfaces/i_edge.hpp"
#include "interfaces/i_graph.hpp"
#include "interfaces/i_vertex.hpp"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {
class Graph : public IGraph {
 public:
  struct Vertex : public IVertex {
   public:
    explicit Vertex(VertexId id);
    VertexId id() const { return id_; }
    GraphDepth depth;
    GraphDepth get_depth() const { return depth; }
    void set_depth(GraphDepth new_depth) { depth = new_depth; }

   private:
    VertexId id_;
  };

  VertexId add_vertex();

  EdgeColor get_new_edge_color(VertexId from_vertex_id, VertexId to_vertex_id);

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

  GraphDepth depth() const { return vertices_on_depth_.size(); }

  GraphDepth get_vertex_depth(VertexId vertex_id) const {
    return vertices_.at(vertex_id)->get_depth();
  }

  const std::unordered_map<VertexId, IVertex*>& get_vertices() const {
    return vertices_;
  }

  const std::unordered_map<EdgeId, IEdge*>& get_edges() const { return edges_; }

  const std::vector<EdgeId>& get_connected_edge_ids(VertexId vertex_id) const {
    return adjacency_list_.at(vertex_id);
  }

  const std::vector<VertexId>& get_vertex_ids_on_depth(
      GraphDepth asked_depth) const;

  bool has_vertex(VertexId vertex_id) const {
    return vertices_.find(vertex_id) != vertices_.end();
  }
  bool has_edge(VertexId from_vertex_id, VertexId to_vertex_id) const;

 private:
  struct Edge : public IEdge {
   public:
    Edge(EdgeId id,
         VertexId from_vertex_id,
         VertexId to_vertex_id,
         EdgeColor color)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id),
          color_(color) {}
    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; }
    VertexId to_vertex_id() const { return to_vertex_id_; }
    EdgeColor color() const { return color_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
    EdgeColor color_ = EdgeColor::Grey;
  };
  std::unordered_map<VertexId, IVertex*> vertices_;
  std::unordered_map<EdgeId, IEdge*> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<GraphDepth, std::vector<VertexId>> vertices_on_depth_;

  VertexId next_vertex_id_ = 0;
  EdgeId next_edge_id_ = 0;

  VertexId generate_vertex_id() { return next_vertex_id_++; }
  EdgeId generate_edge_id() { return next_edge_id_++; }

  void set_vertex_depth(VertexId vertex_id, GraphDepth new_depth);
};

static constexpr GraphDepth kInitialDepth = 1;

}  // namespace uni_course_cpp
