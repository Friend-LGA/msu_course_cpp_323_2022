#include <iostream>
#include <utility>
#include <vector>

#define kVerticesCount 13

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;

  Graph() {}

  struct Edge {
    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id) {}

    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; }
    VertexId to_vertex_id() const { return to_vertex_id_; }

   private:
    const EdgeId id_ = 0;
    const EdgeId from_vertex_id_ = 0;
    const EdgeId to_vertex_id_ = 0;
  };

  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }

   private:
    const VertexId id_ = 0;
  };

  void add_vertex() {
    vertices.push_back(Vertex(new_vertex_id));
    new_vertex_id++;
  }

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    edges.push_back(Edge(new_edge_id, from_vertex_id, to_vertex_id));
    new_edge_id++;
  }

 private:
  std::vector<Vertex> vertices;
  std::vector<Edge> edges;
  EdgeId new_edge_id = 0;
  VertexId new_vertex_id = 0;
};

int main() {
  auto graph = Graph();

  for (int i = 1; i < kVerticesCount; i++) {
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
