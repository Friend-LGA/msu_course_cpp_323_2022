#include "graph.hpp"
#include <cassert>
#include <iostream>
#include "printing.hpp"

void Graph::add_vertex() {
  const VertexId new_id = get_new_vertex_id();
  vertices_.emplace(new_id, new_id);
  adjacency_list_.emplace(new_id, std::vector<EdgeId>{});
}

void Graph::add_edge(VertexId first_vertex_id, VertexId second_vertex_id) {
  assert(has_vertex(first_vertex_id));
  assert(has_vertex(second_vertex_id));
  const EdgeId new_id = get_new_edge_id();

  edges_.try_emplace(new_id, new_id, first_vertex_id, second_vertex_id);
  adjacency_list_[first_vertex_id].emplace_back(new_id);
  if (first_vertex_id != second_vertex_id) {
    adjacency_list_[second_vertex_id].emplace_back(new_id);
  }
}
