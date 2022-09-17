#include "graph.hpp"

struct Graph::Edge {
 public:
  Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
      : id_(id), from_vertex_id_(from_vertex_id), to_vertex_id_(to_vertex_id) {}

  bool operator<(const Edge& rv) const { return rv.id_ > id_; }

  EdgeId id() const { return id_; }
  VertexId from_vertex_id() const { return from_vertex_id_; }
  VertexId to_vertex_id() const { return to_vertex_id_; }

 private:
  EdgeId id_ = 0;
  VertexId from_vertex_id_ = 0;
  VertexId to_vertex_id_ = 0;
};

struct Graph::Vertex {
 public:
  explicit Vertex(VertexId id) : id_(id) {}
  explicit Vertex(const Vertex& vertex) : id_(vertex.id_) {}

  bool operator<(const Vertex& vertex) const { return vertex.id_ > id_; };
  bool operator==(const Vertex& vertex) const { return vertex.id_ == id_; };

  VertexId id() const { return id_; };

 private:
  VertexId id_;
};

void Graph::add_vertex() {
  vertex_map_.insert(
      std::make_pair(Vertex(vertex_map_.size()), std::set<Edge>()));
}

void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  if (vertex_map_.find(Vertex(from_vertex_id)) == vertex_map_.end() ||
      vertex_map_.find(Vertex(to_vertex_id)) == vertex_map_.end()) {
    return;
  }

  auto edge = Edge(next_edge_id_++, from_vertex_id, to_vertex_id);
  vertex_map_[Vertex(from_vertex_id)].insert(edge);
  vertex_map_[Vertex(to_vertex_id)].insert(edge);
}

const int kVerticesCount = 13;

int main() {
  auto graph = Graph();

  for (int i = 0; i < kVerticesCount; i++) {
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
