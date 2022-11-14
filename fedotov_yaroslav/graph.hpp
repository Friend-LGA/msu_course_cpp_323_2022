#pragma once
#include <unordered_map>
#include <vector>

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;

  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id){};
    VertexId id() const { return id_; };

   private:
    VertexId id_ = 0;
  };
  struct Edge {
   public:
    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id){};

    EdgeId id() const { return id_; };

    VertexId from_vertex_id() const { return from_vertex_id_; };

    VertexId to_vertex_id() const { return to_vertex_id_; };

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
  };

  void add_vertex();

  void add_edge(VertexId first_vertex_id, VertexId second_vertex_id);

  const std::unordered_map<VertexId, Vertex>& vertices() const {
    return vertices_;
  };

  const std::unordered_map<EdgeId, Edge>& edges() const { return edges_; };

  const std::vector<EdgeId>& connected_edge_ids(VertexId vertex_id) const {
    return adjacency_list_.at(vertex_id);
  }

 private:
  bool has_vertex(VertexId id) const {
    return vertices_.find(id) != vertices_.end();
  };

  VertexId get_new_vertex_id() { return last_vertex_id_++; };

  EdgeId get_new_edge_id() { return last_edge_id_++; };

  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
};
