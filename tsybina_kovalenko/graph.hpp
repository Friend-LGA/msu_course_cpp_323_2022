#pragma once

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace uni_course_cpp {

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  using Depth = int;

  static constexpr Graph::Depth kGraphBaseDepth = 1;
  static constexpr Graph::Depth kRedEdgeDepthJump = 2;
  static constexpr Graph::Depth kYellowEdgeDepthJump = 1;

  VertexId add_vertex();

  bool has_vertex(VertexId vertex_id) const {
    return vertices_.find(vertex_id) != vertices_.end();
  }

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

  const auto& vertices() const { return vertices_; }

  const auto& edges() const { return edges_; }

  const auto& connections_of(VertexId vertex_id) const {
    return connections_.at(vertex_id);
  }

  Graph::Depth depth() const { return depth_map_.size() - 1; }

  Depth depth_of(VertexId vertex_id) const {
    return vertex_depths_.at(vertex_id);
  }

  const std::unordered_set<VertexId>& vertices_at_depth(Depth depth) const {
    assert(depth < depth_map_.size());
    return depth_map_.at(depth);
  }

  std::vector<VertexId> connected_vertices(VertexId vertex_id) const;

  bool is_connected(VertexId from_vertex_id, VertexId to_vertex_id) const;

  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }

   private:
    VertexId id_ = 0;
  };

  struct Edge {
   public:
    enum class Color { Grey, Green, Yellow, Red };

    explicit Edge(EdgeId id,
                  VertexId from_vertex_id,
                  VertexId to_vertex_id,
                  Color color)
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
    Color color_;
  };

  const std::vector<EdgeId> get_colored_edges_ids(Graph::Edge::Color) const;

 private:
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<Edge::Color, std::vector<EdgeId>> colored_edge_ids_;
  std::unordered_map<VertexId, std::vector<EdgeId>> connections_;
  std::unordered_map<VertexId, Depth> vertex_depths_;
  std::vector<std::unordered_set<VertexId>> depth_map_;

  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;

  VertexId get_new_vertex_id() { return last_vertex_id_++; }
  EdgeId get_new_edge_id() { return last_edge_id_++; }

  Edge::Color define_edge_color(VertexId from_vertex_id,
                                VertexId to_vertex_id) const;

  void set_vertex_depth(VertexId vertex_id, Depth depth);

  VertexId other_end_of(EdgeId edge_id, VertexId vertex_id) const;
};
}  // namespace uni_course_cpp
