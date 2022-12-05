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

  VertexId add_vertex() {
    const VertexId new_vertex_id = generate_vertex_id();
    vertices_.emplace(new_vertex_id, new_vertex_id);

    return new_vertex_id;
  }

  EdgeId add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    const EdgeId new_edge_id = generate_edge_id();

    edges_.emplace(new_edge_id, Edge(new_edge_id, new_edge_id, to_vertex_id));

    return new_edge_id;
  }

 private:
  EdgeId next_edge_id_ = 0;
  VertexId next_vertex_id_ = 0;
  EdgeId generate_edge_id() { return next_edge_id_++; }
  VertexId generate_vertex_id() { return next_vertex_id_++; }

  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, Vertex> vertices_;
};
