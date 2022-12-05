#pragma once

#include <unordered_map>
#include <vector>

class Graph {
 public:
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
    Edge(EdgeId id, VertexId first_vertex_id, VertexId second_vertex_id)
        : id_(id),
          first_vertex_id_(first_vertex_id),
          second_vertex_id_(second_vertex_id) {}

    EdgeId id() const { return id_; }
    VertexId get_first_vertex_id() const { return first_vertex_id_; }
    VertexId get_second_vertex_id() const { return second_vertex_id_; }

   private:
    EdgeId id_ = 0;
    VertexId first_vertex_id_ = 0;
    VertexId second_vertex_id_ = 0;
  };

  void add_vertex();

  void add_edge(VertexId first_vertex_id, VertexId second_vertex_id);

  const std::unordered_map<EdgeId, Edge>& get_edges() const { return edges_; }

  const std::unordered_map<VertexId, Vertex>& get_vertices() const {
    return vertices_;
  }

  const std::vector<EdgeId>& get_connected_edge_ids(VertexId vertex_id) const {
    return connections_.at(vertex_id);
  }

 private:
  VertexId get_new_vertex_id() { return last_vertex_id_++; }

  EdgeId get_new_edge_id() { return last_edge_id_++; };

  bool has_vertex_id(VertexId vertex_id) const {
    return vertices_.find(vertex_id) != vertices_.end();
  }

  std::unordered_map<VertexId, Vertex> vertices_ = {};
  std::unordered_map<EdgeId, Edge> edges_ = {};

  std::unordered_map<VertexId, std::vector<EdgeId>> connections_ = {};
  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
};
