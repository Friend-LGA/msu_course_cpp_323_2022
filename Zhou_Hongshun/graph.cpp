#include <iostream>
#include <vector>
using namespace std;
class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  void add_vertex();
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

 private:
  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }
    VertexId get_new_vertex_id() { return last_vertex_id_++; }

   private:
    VertexId id_ = 0;
    size_t last_vertex_id_ = 0;
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
    EdgeId id_;
    VertexId from_vertex_id_;
    VertexId to_vertex_id_;
  };

  VertexId get_new_vertex_id() { return last_vertex_id_++; }
  EdgeId get_new_edge_id() { return last_edge_id_++; }

  vector<Vertex> vertexes_;
  vector<Edge> edges_;
  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
};
void Graph::add_vertex() {
  vertexes_.emplace_back(Graph::get_new_vertex_id());
}
void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  edges_.emplace_back(Graph::get_new_edge_id(), from_vertex_id, to_vertex_id);
}
constexpr int kVertexesCount = 14;
int main() {
  Graph graph = Graph();
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
