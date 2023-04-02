#include "graph.hpp"

namespace uni_course_cpp {
Graph::VertexId Graph::add_vertex() {
  const VertexId new_vertex_id = get_new_vertex_id();
  vertices_.emplace(new_vertex_id, new_vertex_id);
  connections_[new_vertex_id] = {};
  set_vertex_depth(new_vertex_id, kGraphBaseDepth);
  return new_vertex_id;
}

void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  assert(has_vertex(from_vertex_id));
  assert(has_vertex(to_vertex_id));

  const auto edge_id = get_new_edge_id();
  const auto edge_color = define_edge_color(from_vertex_id, to_vertex_id);
  edges_.emplace(edge_id,
                 Edge(edge_id, from_vertex_id, to_vertex_id, edge_color));

  connections_[from_vertex_id].push_back(edge_id);
  if (to_vertex_id != from_vertex_id) {
    connections_[to_vertex_id].push_back(edge_id);
  }

  if (edge_color == Edge::Color::Grey) {
    set_vertex_depth(to_vertex_id, depth_of(from_vertex_id) + 1);
  }

  colored_edge_ids_[edge_color].push_back(edge_id);
}

std::vector<Graph::VertexId> Graph::connected_vertices(
    VertexId vertex_id) const {
  std::vector<VertexId> result;
  result.reserve(connections_.at(vertex_id).size());
  for (EdgeId edge_id : connections_.at(vertex_id)) {
    result.push_back(other_end_of(edge_id, vertex_id));
  }
  return result;
}

bool Graph::is_connected(VertexId from_vertex_id, VertexId to_vertex_id) const {
  const std::vector<VertexId>& from_vertex_ids =
      connected_vertices(from_vertex_id);
  return std::find(from_vertex_ids.begin(), from_vertex_ids.end(),
                   to_vertex_id) != from_vertex_ids.end();
}

Graph::Edge::Color Graph::define_edge_color(VertexId from_vertex_id,
                                            VertexId to_vertex_id) const {
  if (from_vertex_id == to_vertex_id) {
    return Edge::Color::Green;
  }
  if (depth_of(to_vertex_id) == kGraphBaseDepth) {
    return Edge::Color::Grey;
  }
  const Depth depth_jump = depth_of(to_vertex_id) - depth_of(from_vertex_id);
  if (depth_jump == kYellowEdgeDepthJump &&
      !is_connected(from_vertex_id, to_vertex_id)) {
    return Edge::Color::Yellow;
  }
  if (depth_jump == kRedEdgeDepthJump) {
    return Edge::Color::Red;
  }
  throw std::runtime_error("Failed to determine color");
}

void Graph::set_vertex_depth(VertexId vertex_id, Depth depth) {
  const auto depth_iterator = vertex_depths_.find(vertex_id);
  if (depth_iterator != vertex_depths_.end()) {
    Depth old_depth = depth_iterator->second;
    assert(depth != old_depth);
    depth_map_[old_depth].erase(vertex_id);
  }

  vertex_depths_[vertex_id] = depth;
  if (depth_map_.size() <= depth) {
    depth_map_.resize(depth + 1);
  }
  depth_map_[depth].insert(vertex_id);
}

Graph::VertexId Graph::other_end_of(EdgeId edge_id, VertexId vertex_id) const {
  const Edge& edge = edges().at(edge_id);
  assert(edge.from_vertex_id() == vertex_id ||
         edge.to_vertex_id() == vertex_id);
  if (edge.from_vertex_id() == vertex_id) {
    return edge.to_vertex_id();
  }
  return edge.from_vertex_id();
}

const std::vector<Graph::EdgeId> Graph::get_colored_edges_ids(
    Graph::Edge::Color color) const {
  if (colored_edge_ids_.find(color) != colored_edge_ids_.end()) {
    return colored_edge_ids_.at(color);
  } else {
    static const std::vector<EdgeId> empty_vector_;
    return empty_vector_;
  }
}
}  // namespace uni_course_cpp
