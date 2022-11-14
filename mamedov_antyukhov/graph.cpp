#include "graph.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

Graph::VertexId Graph::add_vertex() {
  const VertexId new_vertex_id = get_new_vertex_id();
  vertices_.insert({new_vertex_id, Vertex(new_vertex_id)});
  set_vertex_depth(new_vertex_id, 1);
  connections_[new_vertex_id] = {};
  return new_vertex_id;
}

const std::vector<Graph::VertexId>& Graph::get_vertex_ids_at_depth(
    Depth depth) const {
  assert(depth < Graph::depth());
  return depth_levels_[depth];
}

Graph::EdgeId Graph::add_edge(VertexId first_vertex_id,
                              VertexId second_vertex_id) {
  assert(has_vertex_id(first_vertex_id));
  assert(has_vertex_id(second_vertex_id));

  const auto first_vertex_depth = get_vertex_depth(first_vertex_id);
  const auto second_vertex_depth = get_vertex_depth(second_vertex_id);

  if (second_vertex_depth == 1) {
    update_vertex_depth(second_vertex_id, first_vertex_depth + 1);
  }
  const auto color = get_edge_color(first_vertex_id, second_vertex_id);
  const EdgeId new_edge_id = get_new_edge_id();

  if (first_vertex_id != second_vertex_id) {
    connections_[second_vertex_id].emplace_back(new_edge_id);
  }
  connections_[first_vertex_id].emplace_back(new_edge_id);

  edges_.insert({new_edge_id,
                 Edge(new_edge_id, first_vertex_id, second_vertex_id, color)});

  return new_edge_id;
}

Graph::Depth Graph::get_vertex_depth(VertexId vertex_id) const {
  assert(has_vertex_id(vertex_id));
  if (vertices_depth_.find(vertex_id) != vertices_depth_.end()) {
    return vertices_depth_.at(vertex_id);
  }
  return 0;
}

void Graph::set_vertex_depth(VertexId vertex_id, Depth new_vertex_depth) {
  vertices_depth_[vertex_id] = new_vertex_depth;
  --new_vertex_depth;
  if (new_vertex_depth < depth_levels_.size()) {
    depth_levels_[new_vertex_depth].emplace_back(vertex_id);
  } else {
    depth_levels_.emplace_back(std::vector<VertexId>{vertex_id});
  }
}

void Graph::update_vertex_depth(VertexId vertex_id, Depth new_vertex_depth) {
  depth_levels_[0].erase(
      std::find(depth_levels_[0].begin(), depth_levels_[0].end(), vertex_id));
  set_vertex_depth(vertex_id, new_vertex_depth);
}

const Graph::Edge::Color Graph::get_edge_color(
    VertexId first_vertex_id,
    VertexId second_vertex_id) const {
  const auto first_vertex_depth = get_vertex_depth(first_vertex_id);
  const auto second_vertex_depth = get_vertex_depth(second_vertex_id);

  if (first_vertex_id == second_vertex_id)
    return Graph::Edge::Color::Green;
  if ((second_vertex_depth - first_vertex_depth) == 1) {
    if (connections_.find(second_vertex_id) != connections_.end())
      return Graph::Edge::Color::Yellow;
    else
      return Graph::Edge::Color::Grey;
  }
  if ((second_vertex_depth - first_vertex_depth) == 2)
    return Graph::Edge::Color::Red;
  throw std::runtime_error("Failed to define color");
}

bool Graph::has_edge(VertexId first_vertex_id,
                     VertexId second_vertex_id) const {
  if (connections_.find(first_vertex_id) == connections_.end())
    return false;
  if (connections_.find(second_vertex_id) == connections_.end())
    return false;
  if (first_vertex_id != second_vertex_id) {
    const auto& first_vertex_edge_ids = get_connected_edge_ids(first_vertex_id);
    const auto& second_vertex_edge_ids =
        get_connected_edge_ids(second_vertex_id);
    return std::find_first_of(
               first_vertex_edge_ids.begin(), first_vertex_edge_ids.end(),
               second_vertex_edge_ids.begin(),
               second_vertex_edge_ids.end()) != first_vertex_edge_ids.end();
  } else {
    const auto& edge_ids = get_connected_edge_ids(first_vertex_id);
    for (const auto edge : edge_ids) {
      if (edges_.at(edge).get_first_vertex_id() ==
          edges_.at(edge).get_second_vertex_id()) {
        return true;
      }
    }
    return false;
  }
}
