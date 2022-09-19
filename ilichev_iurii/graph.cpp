#include "graph.hpp"

#include <cassert>
#include <iostream>

void Graph::add_vertex() {
  const auto next_vertex_id = VertexId(vertexes_.size());

  vertexes_.emplace_back(next_vertex_id);
  vertex_edges_.insert({next_vertex_id, {}});
}

void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  // Checking that an edge can be added between these vertices
  assert(edge_correctness(from_vertex_id, to_vertex_id));

  const auto next_edge_id = EdgeId(edges_.size());

  edges_.emplace_back(next_edge_id, from_vertex_id, to_vertex_id);
  vertex_edges_[from_vertex_id].push_back(next_edge_id);
  vertex_edges_[to_vertex_id].push_back(next_edge_id);
}

// Checking that an edge can be added between the vertices
bool Graph::edge_correctness(VertexId from_vertex_id, VertexId to_vertex_id) {
  if (from_vertex_id >= vertexes_.size() || to_vertex_id >= vertexes_.size()) {
    return false;
  }

  // Checking that there is only one edge between two vertices
  // (includes the case of one loop for a vertex)
  for (const auto& edge_id : vertex_edges_[from_vertex_id]) {
    if (edges_[edge_id].from_vertex_id() == to_vertex_id ||
        edges_[edge_id].to_vertex_id() == to_vertex_id) {
      return false;
    }
  }

  return true;
}

// Prints a list of the IDs of vertices and edges
// that are associated with this vertex
void Graph::print_graph_info() {
  if (!vertex_edges_.size()) {
    std::cout << "empty graph" << std::endl;
    return;
  }

  for (const auto& vertex : vertex_edges_) {
    std::cout << vertex.first << ": ";

    if (!vertex.second.size()) {
      std::cout << "vertex has no edges" << std::endl;
    } else {
      for (const auto& edge_id : vertex.second) {
        std::cout << edge_id << " ";
      }
      std::cout << std::endl;
    }
  }
}

Graph::Edge::Edge() : id_(-1), from_vertex_id_(-1), to_vertex_id_(-1) {}

Graph::Edge::Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
    : id_(id), from_vertex_id_(from_vertex_id), to_vertex_id_(to_vertex_id) {}

Graph::EdgeId Graph::Edge::id() const {
  return id_;
}

Graph::VertexId Graph::Edge::from_vertex_id() const {
  return from_vertex_id_;
}

Graph::VertexId Graph::Edge::to_vertex_id() const {
  return to_vertex_id_;
}

Graph::Vertex::Vertex(Graph::VertexId id) : id_(id) {}

Graph::VertexId Graph::Vertex::id() const {
  return id_;
};
