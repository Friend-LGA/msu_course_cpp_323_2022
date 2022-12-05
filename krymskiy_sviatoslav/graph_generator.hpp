#pragma once

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;

  struct Vertex {
   public:
    explicit Vertex(int id) : id_(id) {}
    VertexId id() const { return id_; }

   private:
    VertexId id_ = 0;
  };

  struct Edge {
   public:
    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id) {}

    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; }
    VertexId to_vertex_id() const { return to_vertex_id_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
  };

  void add_vertex();
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);
  const std::unordered_map<EdgeId, Edge>& get_edges() const;
  const std::unordered_map<VertexId, Vertex>& get_vertices() const;
  const std::vector<EdgeId>& get_vertex_edge_ids(VertexId) const;

 private:
  VertexId get_new_edge_id();
  EdgeId get_new_vertex_id();
  bool has_vertex(VertexId id) const;

  VertexId current_vertex_id_ = 0;
  EdgeId current_edge_id_ = 0;
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
};
