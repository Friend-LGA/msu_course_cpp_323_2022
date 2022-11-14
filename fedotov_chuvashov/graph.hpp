#pragma once
#include <set>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {
class Graph {
 public:
  using Depth = int;
  using VertexId = int;
  using EdgeId = int;
  static constexpr Graph::Depth kGraphBaseDepth = 1;

  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id){};
    VertexId id() const { return id_; };

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

    EdgeId id() const { return id_; };

    VertexId from_vertex_id() const { return from_vertex_id_; };

    VertexId to_vertex_id() const { return to_vertex_id_; };

    Edge::Color color() const { return color_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
    Edge::Color color_ = Color::Grey;
  };

  VertexId add_vertex();

  EdgeId add_edge(VertexId first_vertex_id, VertexId second_vertex_id);

  const std::unordered_map<VertexId, Vertex>& vertices() const {
    return vertices_;
  };

  const std::unordered_map<EdgeId, Edge>& edges() const { return edges_; };

  const std::vector<EdgeId>& connected_edge_ids(VertexId vertex_id) const {
    return adjacency_list_.at(vertex_id);
  }

  bool is_connected(VertexId from_vertex_id, VertexId to_vertex_id) const;

  Edge::Color get_edge_color(VertexId from_vertex_id,
                             VertexId to_vertex_id) const;

  Depth get_vertex_depth(VertexId vertex_id) const {
    return depths_of_vertices_.at(vertex_id);
  }

  void set_vertex_depth(VertexId vertex_id, Depth new_depth);

  const std::vector<EdgeId>& get_colored_edge_ids(Edge::Color color) const;

  Depth depth() const { return vertices_at_depth_.size(); }

  const std::set<VertexId>& vertices_at_depth(Depth depth) const {
    return vertices_at_depth_.at(depth - kGraphBaseDepth);
  }

  std::set<VertexId> children_of_vertex(VertexId vertex_id) const;

 private:
  std::set<VertexId>& get_vertex_ids_at_depth(Depth depth) {
    return vertices_at_depth_[depth - kGraphBaseDepth];
  }

  bool has_vertex(VertexId id) const {
    return vertices_.find(id) != vertices_.end();
  };

  VertexId get_new_vertex_id() { return last_vertex_id_++; };

  EdgeId get_new_edge_id() { return last_edge_id_++; };

  std::unordered_map<Edge::Color, std::vector<EdgeId>> colored_edge_ids_;
  std::unordered_map<VertexId, Depth> depths_of_vertices_;
  std::vector<std::set<VertexId>> vertices_at_depth_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
};

constexpr Graph::Depth kYellowDepthDifference = 1;
constexpr Graph::Depth kRedDepthDifference = 2;
constexpr Graph::Depth kGraphBaseDepth = 1;
constexpr Graph::Depth kGreyDepthDifference = 1;

}  // namespace uni_course_cpp
