#include "graph.hpp"
#include "graph_json_printing.hpp"

namespace uni_course_cpp {

Graph::VertexId Graph::add_vertex() {
  const VertexId new_vertex_id = get_new_vertex_id();
  vertices_.emplace_back(new_vertex_id);
  adjacency_list_[new_vertex_id] = {};

  vertex_depths_[new_vertex_id] = kBaseDepth;
  depth_to_vertices_[kBaseDepth].emplace_back(new_vertex_id);
  return new_vertex_id;
}

Graph::EdgeId Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  assert(has_vertex(from_vertex_id));
  assert(has_vertex(to_vertex_id));

  const auto color = get_edge_color(from_vertex_id, to_vertex_id);
  if (color == Edge::Color::Grey) {
    set_vertex_depth(to_vertex_id, vertex_depth(from_vertex_id) + 1);
  }
  const EdgeId edge_id = get_new_edge_id();
  edges_.emplace_back(edge_id, from_vertex_id, to_vertex_id, color);
  if (from_vertex_id != to_vertex_id) {
    adjacency_list_[from_vertex_id].emplace_back(edge_id);
  }
  adjacency_list_[to_vertex_id].emplace_back(edge_id);
  color_to_edges_[color].emplace_back(edge_id);
  return edge_id;
}

const std::vector<Graph::EdgeId>& Graph::connected_edges_ids(
    VertexId vertex_id) const {
  if (!has_vertex(vertex_id)) {
    static std::vector<EdgeId> empty_edges_list;
    return empty_edges_list;
  }
  return adjacency_list_.at(vertex_id);
}

void Graph::set_vertex_depth(VertexId vertex_id, Depth depth) {
  depth_to_vertices_[depth].emplace_back(vertex_id);
  const auto pos = std::find(depth_to_vertices_[kBaseDepth].begin(),
                             depth_to_vertices_[kBaseDepth].end(), vertex_id);
  if (pos != depth_to_vertices_[kBaseDepth].end()) {
    depth_to_vertices_[kBaseDepth].erase(pos);
  }
  vertex_depths_[vertex_id] = depth;
}

bool Graph::is_connected(VertexId from_vertex_id, VertexId to_vertex_id) const {
  const auto& edges_ids = adjacency_list_.at(from_vertex_id);
  for (const auto& edge_id : edges_ids) {
    const auto& edge = edges_[edge_id];
    if (edge.from_vertex_id() == to_vertex_id ||
        edge.to_vertex_id() == to_vertex_id) {
      return true;
    }
  }
  return false;
}

Graph::Edge::Color Graph::get_edge_color(VertexId from_vertex_id,
                                         VertexId to_vertex_id) const {
  const auto from_vertex_depth = vertex_depths_.at(from_vertex_id);
  const auto to_vertex_depth = vertex_depths_.at(to_vertex_id);

  if (from_vertex_id == to_vertex_id) {
    return Edge::Color::Green;
  }
  if (adjacency_list_.at(to_vertex_id).size() == 0) {
    return Edge::Color::Grey;
  }
  if (to_vertex_depth - from_vertex_depth == kDifferenceYellowEdge &&
      !is_connected(from_vertex_id, to_vertex_id)) {
    return Edge::Color::Yellow;
  }
  if (to_vertex_depth - from_vertex_depth == kDifferenceRedEdge) {
    return Edge::Color::Red;
  }
  throw std::runtime_error("Failed to determine color");
}

const std::vector<Graph::VertexId>& Graph::get_vertices_with_depth(
    Depth depth) const {
  if (depth_to_vertices_.find(depth) != depth_to_vertices_.end()) {
    return depth_to_vertices_.at(depth);
  } else {
    static std::vector<VertexId> tmp;
    return tmp;
  }
}

const std::vector<Graph::EdgeId>& Graph::get_edge_ids_with_color(
    Edge::Color color) const {  // ????
  if (color_to_edges_.find(color) != color_to_edges_.end()) {
    return color_to_edges_.at(color);
  } else {
    static std::vector<VertexId> tmp;
    return tmp;
  }
}

}  // namespace uni_course_cpp
