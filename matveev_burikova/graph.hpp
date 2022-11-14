#pragma once

#include "config.hpp"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {
class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  using Depth = int;

  struct Vertex {
   public:
    explicit Vertex(VertexId id);
    VertexId id() const { return id_; }
    Depth depth;

   private:
    VertexId id_;
  };

  struct Edge {
   public:
    enum class Color { Grey, Green, Yellow, Red };
    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id, Color color)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id),
          color_(color) {}
    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; }
    VertexId to_vertex_id() const { return to_vertex_id_; }
    Color color() const { return color_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
    Color color_ = Color::Grey;
  };

  VertexId add_vertex();

  Edge::Color get_new_edge_color(VertexId from_vertex_id,
                                 VertexId to_vertex_id);

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

  Depth depth() const { return vertices_on_depth_.size(); }

  Depth get_vertex_depth(VertexId vertex_id) const {
    return vertices_.at(vertex_id).depth;
  }

  const std::unordered_map<VertexId, Vertex>& get_vertices() const {
    return vertices_;
  }

  const std::unordered_map<EdgeId, Edge>& get_edges() const { return edges_; }

  const std::vector<EdgeId>& get_connected_edge_ids(VertexId vertex_id) const {
    return adjacency_list_.at(vertex_id);
  }

  const std::vector<VertexId>& get_vertex_ids_on_depth(Depth asked_depth) const;

  bool has_vertex(VertexId vertex_id) const {
    return vertices_.find(vertex_id) != vertices_.end();
  }
  bool has_edge(VertexId from_vertex_id, VertexId to_vertex_id) const;

 private:
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<Depth, std::vector<VertexId>> vertices_on_depth_;

  VertexId next_vertex_id_ = 0;
  EdgeId next_edge_id_ = 0;

  VertexId generate_vertex_id() { return next_vertex_id_++; }
  EdgeId generate_edge_id() { return next_edge_id_++; }

  void set_vertex_depth(VertexId vertex_id, Depth new_depth);
};

static constexpr Graph::Depth kInitialDepth = 1;

}  // namespace uni_course_cpp
