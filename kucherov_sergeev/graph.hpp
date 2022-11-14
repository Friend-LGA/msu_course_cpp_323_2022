#pragma once

#include <unordered_map>
#include <vector>

namespace uni_course_cpp {
class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  using Depth = int;

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

  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }

   private:
    VertexId id_ = 0;
  };

  VertexId add_vertex();

  EdgeId add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

  Depth get_depth() const;

  const std::vector<VertexId>& get_depth_vertex_ids(Depth depth) const;

  const std::vector<EdgeId>& get_connected_edge_ids(VertexId vertex_id) const;

  bool is_vertices_connected(VertexId first_vertex_id,
                             VertexId second_vertex_id) const;

  Depth get_vertex_depth(VertexId vertex_id) const;

  const std::unordered_map<Graph::VertexId, Graph::Vertex>& get_vertices()
      const;

  const std::unordered_map<Graph::EdgeId, Graph::Edge>& get_edges() const;

 private:
  VertexId get_new_vertex_id();

  EdgeId get_new_edge_id();

  Edge::Color determine_edge_color(VertexId from_vertex_id,
                                   VertexId to_vertex_id) const;

  void set_vertex_depth(VertexId vertex_id, Depth depth);

  VertexId next_free_vertex_id_ = 0;
  EdgeId next_free_edge_id_ = 0;
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<VertexId, Depth> vertex_depths_list_;
  std::vector<std::vector<VertexId>> depth_vertices_list_ = {{}};
};

static constexpr Graph::Depth kGraphDefaultDepth = 1;

}  // namespace uni_course_cpp
