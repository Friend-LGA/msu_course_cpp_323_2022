#include <algorithm>
#include <stdexcept>

#include "graph.hpp"

namespace uni_course_cpp {
Graph::VertexId Graph::add_vertex() {
  const VertexId vertex_id = get_new_vertex_id();

  vertices_.insert({vertex_id, Graph::Vertex(vertex_id)});
  set_vertex_depth(vertex_id, kGraphDefaultDepth);

  return vertex_id;
}

Graph::EdgeId Graph::add_edge(Graph::VertexId from_vertex_id,
                              Graph::VertexId to_vertex_id) {
  if (get_connected_edge_ids(to_vertex_id).empty()) {
    set_vertex_depth(to_vertex_id, get_vertex_depth(from_vertex_id) + 1);
  }

  const auto edge_id = get_new_edge_id();
  const auto edge_color = determine_edge_color(from_vertex_id, to_vertex_id);

  edges_.insert({edge_id, Graph::Edge(edge_id, from_vertex_id, to_vertex_id,
                                      edge_color)});

  adjacency_list_[from_vertex_id].push_back(edge_id);
  if (to_vertex_id != from_vertex_id) {
    adjacency_list_[to_vertex_id].push_back(edge_id);
  }

  return edge_id;
}

Graph::Depth Graph::get_depth() const {
  return (depth_vertices_list_.empty()) ? (0)
                                        : (depth_vertices_list_.size() - 1);
}

const std::vector<Graph::VertexId>& Graph::get_depth_vertex_ids(
    Graph::Depth depth) const {
  if (depth > get_depth()) {
    static const std::vector<VertexId> empty_result;
    return empty_result;
  }

  return depth_vertices_list_.at(depth);
}

const std::vector<Graph::EdgeId>& Graph::get_connected_edge_ids(
    Graph::VertexId vertex_id) const {
  if (adjacency_list_.find(vertex_id) == adjacency_list_.end()) {
    static const std::vector<Graph::EdgeId> empty_result;
    return empty_result;
  }

  return adjacency_list_.at(vertex_id);
}

bool Graph::is_vertices_connected(Graph::VertexId first_vertex_id,
                                  Graph::VertexId second_vertex_id) const {
  const auto& connected_edge_ids = get_connected_edge_ids(first_vertex_id);

  for (const auto& edge_id : connected_edge_ids) {
    const auto& current_edge = edges_.at(edge_id);
    if ((first_vertex_id == current_edge.from_vertex_id() &&
         second_vertex_id == current_edge.to_vertex_id()) ||
        (first_vertex_id == current_edge.to_vertex_id() &&
         second_vertex_id == current_edge.from_vertex_id())) {
      return true;
    }
  }

  return false;
}

Graph::Depth Graph::get_vertex_depth(Graph::VertexId vertex_id) const {
  return vertex_depths_list_.at(vertex_id);
}

const std::unordered_map<Graph::VertexId, Graph::Vertex>& Graph::get_vertices()
    const {
  return vertices_;
}

const std::unordered_map<Graph::EdgeId, Graph::Edge>& Graph::get_edges() const {
  return edges_;
}

Graph::VertexId Graph::get_new_vertex_id() {
  return next_free_vertex_id_++;
}

Graph::EdgeId Graph::get_new_edge_id() {
  return next_free_edge_id_++;
}

Graph::Edge::Color Graph::determine_edge_color(
    Graph::VertexId from_vertex_id,
    Graph::VertexId to_vertex_id) const {
  const auto from_vertex_depth = get_vertex_depth(from_vertex_id);
  const auto to_vertex_depth = get_vertex_depth(to_vertex_id);

  if (from_vertex_id == to_vertex_id) {
    return Graph::Edge::Color::Green;
  }
  if (get_connected_edge_ids(to_vertex_id).size() == 0) {
    return Graph::Edge::Color::Grey;
  }
  if (to_vertex_depth - from_vertex_depth == 1 &&
      !is_vertices_connected(from_vertex_id, to_vertex_id)) {
    return Graph::Edge::Color::Yellow;
  }
  if (to_vertex_depth - from_vertex_depth == 2) {
    return Graph::Edge::Color::Red;
  }

  throw std::runtime_error("Failed to determine color");
}

void Graph::set_vertex_depth(Graph::VertexId vertex_id, Graph::Depth depth) {
  while (get_depth() < depth) {
    depth_vertices_list_.push_back({});
  }

  if (vertex_depths_list_.find(vertex_id) != vertex_depths_list_.end()) {
    const Depth previous_depth = get_vertex_depth(vertex_id);

    auto& previous_depth_vertices_list = depth_vertices_list_[previous_depth];
    previous_depth_vertices_list.erase(
        std::remove(previous_depth_vertices_list.begin(),
                    previous_depth_vertices_list.end(), vertex_id),
        previous_depth_vertices_list.end());
  }

  depth_vertices_list_[depth].push_back(vertex_id);
  vertex_depths_list_[vertex_id] = depth;
}
}  // namespace uni_course_cpp
