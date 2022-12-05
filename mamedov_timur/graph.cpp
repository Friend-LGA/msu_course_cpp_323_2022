#include "graph.hpp"
#include <cassert>

void Graph::add_vertex() {
  const VertexId new_vertex_id = get_new_vertex_id();
  vertices_.insert({new_vertex_id, Vertex(new_vertex_id)});
};

void Graph::add_edge(VertexId first_vertex_id, VertexId second_vertex_id) {
  assert(has_vertex_id(first_vertex_id));
  assert(has_vertex_id(second_vertex_id));

  const EdgeId new_edge_id = get_new_edge_id();
  edges_.insert(
      {new_edge_id, Edge(new_edge_id, first_vertex_id, second_vertex_id)});

  if (first_vertex_id != second_vertex_id) {
    connections_[second_vertex_id].emplace_back(new_edge_id);
  }
  connections_[first_vertex_id].push_back(new_edge_id);
};
