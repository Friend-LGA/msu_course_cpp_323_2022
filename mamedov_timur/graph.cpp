#include <iostream>
#include <vector>

class Graph {
  using VertexId = int;
  using EdgeId = int;

 public:
  void add_vertex() { vertexes.emplace_back(vertexes.size()); }

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    edges.emplace_back(edges.size(), from_vertex_id, to_vertex_id);
  }

 private:
  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}

    VertexId id() const { return id_; }

   private:
    VertexId id_{};
  };

  struct Edge {
   public:
    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id) {}

    void print() const {
      std::cout << from_vertex_id_ << " " << to_vertex_id_ << std::endl;
    }

   private:
    EdgeId id_{};

    VertexId from_vertex_id_{};
    VertexId to_vertex_id_{};
  };

  std::vector<Vertex> vertexes;
  std::vector<Edge> edges;
};

int main() {
  auto graph = Graph();
  int kVertexesCount;

  for (int i = 0; i < kVertexesCount; i++) {
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
