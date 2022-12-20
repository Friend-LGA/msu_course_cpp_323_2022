#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  using Depth = int;
  static constexpr Depth kBaseDepth = 1;
  static constexpr Depth kDifferenceRedEdge = 2;
  static constexpr Depth kDifferenceYellowEdge = 1;

  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }

   private:
    VertexId id_ = 0;
  };

  struct Edge {
    enum class Color { Grey, Green, Yellow, Red };

   public:
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
    Color color_ = Color::Grey;
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
  };

  VertexId add_vertex();

  EdgeId add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

  bool has_vertex(VertexId vertex_id) const {
    return adjacency_list_.find(vertex_id) != adjacency_list_.end();
  }

  const std::vector<Vertex>& get_vertices() const { return vertices_; }

  const std::vector<Edge>& get_edges() const { return edges_; }

  const std::vector<EdgeId>& connected_edges_ids(VertexId vertex_id) const;

  void set_vertex_depth(VertexId vertex_id, Depth depth);

  bool is_connected(VertexId from_vertex_id, VertexId to_vertex_id) const;

  Edge::Color get_edge_color(VertexId from_vertex_id,
                             VertexId to_vertex_id) const;

  Depth vertex_depth(VertexId vertex_id) const {
    return vertex_depths_.at(vertex_id);
  }

  const std::vector<VertexId>& get_vertices_with_depth(Depth depth) const;

  const std::vector<Graph::EdgeId>& get_edge_ids_with_color(
      Edge::Color color) const;

  Depth depth() const { return depth_to_vertices_.size(); }

 private:
  VertexId get_new_vertex_id() { return vertex_id_counter_++; }
  EdgeId get_new_edge_id() { return edge_id_counter_++; }

  VertexId vertex_id_counter_ = 0;
  EdgeId edge_id_counter_ = 0;
  std::vector<Vertex> vertices_;
  std::vector<Edge> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId> > adjacency_list_;
  std::unordered_map<VertexId, Depth> vertex_depths_;
  std::unordered_map<Depth, std::vector<VertexId> > depth_to_vertices_;
  std::unordered_map<Edge::Color, std::vector<EdgeId> > color_to_edges_;
};

}  // namespace uni_course_cpp
