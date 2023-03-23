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

  bool is_connected(VertexId, VertexId) const;
  VertexId add_vertex();

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

  Depth get_graph_depth() const {
    return vertexes_of_depth_.size() ? vertexes_of_depth_.size() - 1 : 0;
  }
  Depth get_vertex_depth(VertexId id) const;

  const std::unordered_map<VertexId, Vertex>& get_vertexes() const {
    return vertexes_;
  }

  const std::vector<VertexId>& vertexes_of_depth(Depth depth) const {
    return vertexes_of_depth_[depth];
  };

  const std::unordered_map<EdgeId, Edge>& get_edges() const { return edges_; }
  const std::set<EdgeId>& connected_edge_ids(VertexId vertex_id) const {
    return adjacency_list_.at(vertex_id);
  }

  const std::vector<EdgeId>& edges_ids_of_color(Edge::Color color) const;

  bool has_vertex(VertexId id) const {
    return vertexes_.find(id) != vertexes_.end();
  }
  bool has_edge(EdgeId id) const { return edges_.find(id) != edges_.end(); }

 private:
  void add_edge_in_color(EdgeId edge_id, const Edge::Color& color);
  EdgeId gen_new_edge_id() { return last_edge_id_++; }
  VertexId gen_new_vertex_id() { return last_vertex_id_++; }

  Edge::Color define_color(VertexId from_vertex_id,
                           VertexId to_vertex_id) const;
  void set_vertex_depth(VertexId id, Depth depth);

  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
  std::unordered_map<VertexId, Depth> depth_of_vertexes_;
  std::vector<std::vector<VertexId>> vertexes_of_depth_;
  std::unordered_map<VertexId, Vertex> vertexes_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, std::set<EdgeId>> adjacency_list_;
  std::unordered_map<Edge::Color, std::vector<EdgeId>> edges_ids_of_color_;
};

}  // namespace uni_course_cpp
