#include "graph.hpp"
#include <iostream>
#include <cassert>

struct Graph::Vertex {
 public:
  explicit Vertex(VertexId id) : id_(id) {}
  VertexId id() const { return id_; }
  bool operator<(const Vertex& vertex_) const { return id_ < vertex_.id_; }

 private:
  VertexId id_ = 0;
};

struct Graph::Edge {
 public:
  Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
      : id_(id), from_vertex_id_(from_vertex_id), to_vertex_id_(to_vertex_id) {}

  EdgeId id() const { return id_; }
  VertexId from_vertex_id() const { return from_vertex_id_; }
  VertexId to_vertex_id() const { return to_vertex_id_; }

  bool operator<(const Edge& edge_) const { return id_ < edge_.id_; }

 private:
  EdgeId id_ = 0;
  VertexId from_vertex_id_ = 0;
  VertexId to_vertex_id_ = 0;
};

void Graph::add_vertex() {
  verticies_map_.insert({Vertex(verticies_map_.size()), {}});
}

void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  assert(verticies_map_.find(Vertex(from_vertex_id)) != verticies_map_.end() &&
      verticies_map_.find(Vertex(from_vertex_id)) != verticies_map_.end());

  const Edge new_edge(edges_.size(), from_vertex_id, to_vertex_id);

  verticies_map_[Vertex(from_vertex_id)].insert(new_edge.id());
  verticies_map_[Vertex(to_vertex_id)].insert(new_edge.id());
  edges_.insert(new_edge);
}

void Graph::print_graph_info() {
  for (const auto& vertex : verticies_map_) {
    std::cout << vertex.first.id() << " :";
    for (const auto& edge_id : vertex.second) {
      std::cout << " " << edge_id;
    }
    std::cout << std::endl;
  }
}

static constexpr int kVerticesCount = 13;

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

  graph.print_graph_info();

  return 0;
}
