#pragma once
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
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }

   private:
    VertexId id_ = 0;
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

  EdgeId add_edge(VertexId, VertexId);

  const std::vector<EdgeId>& connected_edge_ids(VertexId id) const {
    return adjacency_list_.at(id);
  }

  const std::unordered_map<VertexId, Vertex>& vertices() const {
    return vertices_;
  }

  const std::unordered_map<EdgeId, Edge>& edges() const { return edges_; }

  Depth depth() const;

  Depth vertex_depth(VertexId vertex_id) const { return depths_.at(vertex_id); }

  const std::vector<VertexId>& vertices_at_depth(Depth depth) const {
    return vertices_at_depth_.at(depth);
  }

  bool has_edge(VertexId, VertexId) const;

  const std::vector<EdgeId>& color_edge_ids(Edge::Color color) const;

 private:
  VertexId current_vertex_id_ = 0;
  EdgeId current_edge_id_ = 0;

  VertexId next_vertex_id() { return current_vertex_id_++; }

  EdgeId next_edge_id() { return current_edge_id_++; }

  bool has_vertex(VertexId vertex_id) const {
    return vertices_.find(vertex_id) != vertices_.end();
  }

  void set_vertex_depth(VertexId, Depth);

  Edge::Color determine_color(VertexId, VertexId) const;

  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<VertexId, Depth> depths_;
  std::vector<std::vector<VertexId>> vertices_at_depth_;
  std::unordered_map<Edge::Color, std::vector<EdgeId>> color_edge_ids_;
};

constexpr Graph::Depth kYellowEdgeDepth = 1;
constexpr Graph::Depth kRedEdgeDepth = 2;
constexpr uni_course_cpp::Graph::Depth kDefaultDepth = 1;
}  // namespace uni_course_cpp
