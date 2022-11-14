#include <unordered_map>
#include <vector>

constexpr int kVerticesCount = 14;

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
    explicit Edge(EdgeId id,
                  VertexId first_vertex_id,
                  VertexId second_vertex_id)
        : id_(id),
          first_vertex_id_(first_vertex_id),
          second_vertex_id_(second_vertex_id){};

    EdgeId id() const { return id_; };
    VertexId get_first_vertex_id() const { return first_vertex_id_; };
    VertexId get_second_vertex_id() const { return second_vertex_id_; };

   private:
    EdgeId id_ = 0;
    VertexId first_vertex_id_ = 0;
    VertexId second_vertex_id_ = 0;
  };
  void add_vertex() {
    const VertexId new_id = get_new_vertex_id();
    vertices_.emplace_back(new_id);

    connections_[new_id] = {};
  };

  void add_edge(VertexId first_vertex_id, VertexId second_vertex_id) {
    const EdgeId new_edge_id = get_new_edge_id();
    edges_.emplace_back(new_edge_id, first_vertex_id, second_vertex_id);
    connections_[first_vertex_id].push_back(new_edge_id);
    if (first_vertex_id != second_vertex_id)
      connections_[second_vertex_id].push_back(new_edge_id);
  };

  const std::vector<Edge>& get_edges() const { return edges_; }

  const std::vector<Vertex>& get_vertices() const { return vertices_; }

  const std::unordered_map<VertexId, std::vector<EdgeId>>& get_connections()
      const {
    return connections_;
  }

 private:
  VertexId get_new_vertex_id() { return last_vertex_id_++; };

  EdgeId get_new_edge_id() { return last_edge_id_++; };

  std::vector<Vertex> vertices_ = {};
  std::vector<Edge> edges_ = {};
  std::unordered_map<VertexId, std::vector<EdgeId>> connections_ = {};
  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
};

int main() {
  auto graph = Graph();

  for (int i = 0; i < kVerticesCount; ++i) {
    graph.add_vertex();
  }
  graph.add_edge(0, 1);
  graph.add_edge(0, 2);
  graph.add_edge(0, 3);
  graph.add_edge(1, 4);
  graph.add_edge(1, 5);
  graph.add_edge(1, 6);
  graph.add_edge(2, 7);
  graph.add_edge(2, 8);
  graph.add_edge(3, 9);
  graph.add_edge(4, 10);
  graph.add_edge(5, 10);
  graph.add_edge(6, 10);
  graph.add_edge(7, 11);
  graph.add_edge(8, 11);
  graph.add_edge(9, 12);
  graph.add_edge(10, 13);
  graph.add_edge(11, 13);
  graph.add_edge(12, 13);

  return 0;
}
