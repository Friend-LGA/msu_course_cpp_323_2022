#include "graph.hpp"
#include <cassert>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {
Graph::EdgeId Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  assert(has_vertex(from_vertex_id));
  assert(has_vertex(to_vertex_id));
  assert(!has_edge(from_vertex_id, to_vertex_id));

  if (connected_edge_ids(to_vertex_id).empty()) {
    const auto from_vertex_depth = vertex_depth(from_vertex_id);
    set_vertex_depth(to_vertex_id, from_vertex_depth + 1);
  }

  const auto color = determine_color(from_vertex_id, to_vertex_id);

  const auto edge_id = next_edge_id();
  adjacency_list_[from_vertex_id].push_back(edge_id);
  if (from_vertex_id != to_vertex_id) {
    adjacency_list_[to_vertex_id].push_back(edge_id);
  }

  edges_.try_emplace(edge_id, edge_id, from_vertex_id, to_vertex_id, color);
  color_edge_ids_[color].push_back(edge_id);

  return edge_id;
}

Graph::Depth Graph::depth() const {
  if (!vertices_at_depth_.empty()) {
    return vertices_at_depth_.size() - 1;
  }
  return 0;
}

bool Graph::has_edge(VertexId from_vertex_id, VertexId to_vertex_id) const {
  assert(adjacency_list_.find(from_vertex_id) != adjacency_list_.end());
  assert(adjacency_list_.find(to_vertex_id) != adjacency_list_.end());
  if (from_vertex_id != to_vertex_id) {
    const auto& connected_from_edges_ids = adjacency_list_.at(from_vertex_id);
    const auto& connected_to_edges_ids = adjacency_list_.at(to_vertex_id);
    for (const auto from_edge_id : connected_from_edges_ids) {
      for (const auto to_edge_id : connected_to_edges_ids) {
        if (from_edge_id == to_edge_id) {
          return true;
        }
      }
    }
  } else {
    const auto& edges_ids = adjacency_list_.at(from_vertex_id);
    for (const auto edge_id : edges_ids) {
      if (edges_.at(edge_id).from_vertex_id() ==
          edges_.at(edge_id).to_vertex_id()) {
        return true;
      }
    }
  }
  return false;
}

Graph::VertexId Graph::add_vertex() {
  const auto vertex_id = next_vertex_id();
  vertices_.try_emplace(vertex_id, vertex_id);
  adjacency_list_[vertex_id] = {};
  set_vertex_depth(vertex_id, kDefaultDepth);
  return vertex_id;
}

void Graph::set_vertex_depth(VertexId vertex_id, Depth depth) {
  depths_[vertex_id] = depth;
  if (depth >= vertices_at_depth_.size()) {
    vertices_at_depth_.resize(depth + 1);
  }
  vertices_at_depth_[depth].push_back(vertex_id);
  if (depth != kDefaultDepth) {
    for (auto vertex_id_iterator = vertices_at_depth_[kDefaultDepth].cbegin();
         vertex_id_iterator != vertices_at_depth_[kDefaultDepth].cend();
         ++vertex_id_iterator) {
      if (*vertex_id_iterator == vertex_id) {
        vertices_at_depth_[kDefaultDepth].erase(vertex_id_iterator);
        break;
      }
    }
  }
}

Graph::Edge::Color Graph::determine_color(VertexId from_vertex_id,
                                          VertexId to_vertex_id) const {
  const auto from_vertex_depth = vertex_depth(from_vertex_id);
  const auto to_vertex_depth = vertex_depth(to_vertex_id);
  if (from_vertex_id == to_vertex_id) {
    return Edge::Color::Green;
  }
  if (connected_edge_ids(to_vertex_id).size() == 0) {
    return Edge::Color::Grey;
  }
  if (to_vertex_depth - from_vertex_depth == kYellowEdgeDepth &&
      !has_edge(from_vertex_id, to_vertex_id)) {
    return Edge::Color::Yellow;
  }
  if (to_vertex_depth - from_vertex_depth == kRedEdgeDepth) {
    return Edge::Color::Red;
  }
  throw std::runtime_error("Failed to determine color");
}

const std::vector<Graph::EdgeId>& Graph::color_edge_ids(
    Graph::Edge::Color color) const {
  if (color_edge_ids_.find(color) != color_edge_ids_.end()) {
    return color_edge_ids_.at(color);
  }
  static std::vector<EdgeId> empty_vector;
  return empty_vector;
}
}  // namespace uni_course_cpp
