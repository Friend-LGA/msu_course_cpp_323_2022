#include "graph.hpp"

#include <cassert>

namespace {

using Vertex = uni_course_cpp::Graph::Vertex;

}

namespace uni_course_cpp {

Vertex::Vertex(VertexId id) : id_(id), depth(kInitialDepth) {}

Graph::VertexId Graph::add_vertex() {
  const VertexId new_vertex_id = generate_vertex_id();
  adjacency_list_[new_vertex_id] = {};
  vertices_.emplace(new_vertex_id, Vertex(new_vertex_id));
  vertices_on_depth_[kInitialDepth].emplace_back(new_vertex_id);
  return new_vertex_id;
}

Graph::Edge::Color Graph::get_new_edge_color(VertexId from_vertex_id,
                                             VertexId to_vertex_id) {
  if (from_vertex_id == to_vertex_id) {
    return Edge::Color::Green;
  }
  if (get_connected_edge_ids(to_vertex_id).size() == 0) {
    return Edge::Color::Grey;
  }
  if (get_vertex_depth(to_vertex_id) - get_vertex_depth(from_vertex_id) == 1 &&
      !has_edge(from_vertex_id, to_vertex_id)) {
    return Edge::Color::Yellow;
  }
  if (get_vertex_depth(to_vertex_id) - get_vertex_depth(from_vertex_id) == 2) {
    return Edge::Color::Red;
  }
  throw std::runtime_error("Failed to determine color");
}

const std::vector<Graph::VertexId>& Graph::get_vertex_ids_on_depth(
    Graph::Depth asked_depth) const {
  assert(depth() >= asked_depth);
  return vertices_on_depth_.at(asked_depth);
}

void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  assert(has_vertex(from_vertex_id));
  assert(has_vertex(to_vertex_id));
  assert(!has_edge(from_vertex_id, to_vertex_id));
  const auto from_vertex_depth = get_vertex_depth(from_vertex_id);
  if (get_connected_edge_ids(to_vertex_id).empty()) {
    const auto new_to_vertex_depth = from_vertex_depth + 1;
    set_vertex_depth(to_vertex_id, new_to_vertex_depth);
  }
  const EdgeId new_edge_id = generate_edge_id();
  const Edge::Color color = get_new_edge_color(from_vertex_id, to_vertex_id);
  edges_.emplace(new_edge_id,
                 Edge(new_edge_id, from_vertex_id, to_vertex_id, color));
  adjacency_list_[from_vertex_id].emplace_back(new_edge_id);
  if (from_vertex_id != to_vertex_id) {
    adjacency_list_[to_vertex_id].emplace_back(new_edge_id);
  }
}

bool Graph::has_edge(VertexId from_vertex_id, VertexId to_vertex_id) const {
  for (const auto& [edge_id, edge] : edges_) {
    if ((edge.from_vertex_id() == from_vertex_id &&
         edge.to_vertex_id() == to_vertex_id) ||
        (edge.from_vertex_id() == to_vertex_id &&
         edge.to_vertex_id() == from_vertex_id))
      return true;
  }
  return false;
}

void Graph::set_vertex_depth(VertexId vertex_id, Depth new_depth) {
  const auto old_depth = get_vertex_depth(vertex_id);
  const auto iterator = find(vertices_on_depth_[old_depth].begin(),
                             vertices_on_depth_[old_depth].end(), vertex_id);
  assert(iterator != vertices_on_depth_[old_depth].end());
  vertices_on_depth_[old_depth].erase(iterator);
  vertices_.at(vertex_id).depth = new_depth;
  vertices_on_depth_[new_depth].emplace_back(vertex_id);
}
}  // namespace uni_course_cpp
