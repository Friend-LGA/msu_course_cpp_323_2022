#include "graph.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

namespace uni_course_cpp {

namespace {

static constexpr Graph::Depth kBaseDepth = 1;

}

const std::vector<Graph::EdgeId>& Graph::edges_ids_of_color(
    Graph::Edge::Color color) const {
  if (edges_ids_of_color_.find(color) == edges_ids_of_color_.end()) {
    static const std::vector<EdgeId> empty_result;
    return empty_result;
  }

  return edges_ids_of_color_.at(color);
}

void Graph::add_edge_in_color(Graph::EdgeId edge_id,
                              const Graph::Edge::Color& color) {
  if (edges_ids_of_color_.find(color) == edges_ids_of_color_.end()) {
    std::vector<Graph::EdgeId> empty_vector;
    edges_ids_of_color_.insert(std::make_pair(color, empty_vector));
  }

  auto& set_of_edges = edges_ids_of_color_.at(color);
  set_of_edges.emplace_back(edge_id);
}

Graph::Depth Graph::get_vertex_depth(Graph::VertexId id) const {
  assert(has_vertex(id));
  return depth_of_vertexes_.at(id);
}

Graph::Edge::Color Graph::define_color(Graph::VertexId from_vertex_id,
                                       Graph::VertexId to_vertex_id) const {
  const auto from_vertex_depth = get_vertex_depth(from_vertex_id);
  const auto to_vertex_depth = get_vertex_depth(to_vertex_id);
  Edge::Color color = Edge::Color::Grey;
  if (from_vertex_id == to_vertex_id) {
    color = Edge::Color::Green;
  } else if (to_vertex_depth - from_vertex_depth <= 0) {
    color = Edge::Color::Grey;
  } else if (to_vertex_depth - from_vertex_depth == 1 &&
             !is_connected(to_vertex_id, from_vertex_id)) {
    color = Edge::Color::Yellow;
  } else if (to_vertex_depth - from_vertex_depth == 2) {
    color = Edge::Color::Red;
  } else {
    throw std::runtime_error("Can't define the color");
  }

  return color;
}

Graph::VertexId Graph::add_vertex() {
  const auto vertex_id = gen_new_vertex_id();
  vertexes_.insert(std::make_pair(vertex_id, Vertex(vertex_id)));

  if (vertexes_of_depth_.empty()) {
    std::vector<VertexId> new_empty_vertex = {};
    vertexes_of_depth_.emplace_back(new_empty_vertex);
    vertexes_of_depth_.emplace_back(new_empty_vertex);
  }

  vertexes_of_depth_[kBaseDepth].emplace_back(vertex_id);
  depth_of_vertexes_[vertex_id] = kBaseDepth;
  adjacency_list_[vertex_id] = {};

  return vertex_id;
}

bool Graph::is_connected(Graph::VertexId from_vertex_id,
                         Graph::VertexId to_vertex_id) const {
  if (from_vertex_id == to_vertex_id) {
    for (const auto& edge_id : adjacency_list_.at(from_vertex_id)) {
      if (edges_.at(edge_id).color() == Edge::Color::Green) {
        return true;
      }
    }

    return false;
  }

  const auto& pull_edges_from = adjacency_list_.at(from_vertex_id);
  const auto& pull_edges_to = adjacency_list_.at(to_vertex_id);

  std::set<Graph::EdgeId> intersection;

  std::set_intersection(pull_edges_from.begin(), pull_edges_from.end(),
                        pull_edges_to.begin(), pull_edges_to.end(),
                        std::inserter(intersection, intersection.begin()));

  return !intersection.empty();
}

void Graph::set_vertex_depth(VertexId id, Depth depth) {
  const auto cur_depth = get_vertex_depth(id);
  const auto graph_depth = get_graph_depth();

  if (depth > graph_depth) {
    std::vector<VertexId> empty_vertex = {};
    vertexes_of_depth_.emplace_back(empty_vertex);
  }

  depth_of_vertexes_[id] = depth;
  vertexes_of_depth_[depth].emplace_back(id);
  vertexes_of_depth_[cur_depth].erase(
      std::remove(vertexes_of_depth_[cur_depth].begin(),
                  vertexes_of_depth_[cur_depth].end(), id));
}

void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  assert(has_vertex(from_vertex_id));
  assert(has_vertex(to_vertex_id));
  const auto edge_id = gen_new_edge_id();
  const auto edge_color = define_color(from_vertex_id, to_vertex_id);

  add_edge_in_color(edge_id, edge_color);

  if (edge_color == Graph::Edge::Color::Grey) {
    set_vertex_depth(to_vertex_id, get_vertex_depth(from_vertex_id) + 1);
  }

  edges_.insert(std::make_pair(
      edge_id, Edge(edge_id, from_vertex_id, to_vertex_id, edge_color)));
  if (from_vertex_id != to_vertex_id) {
    adjacency_list_[from_vertex_id].insert(edge_id);
  }

  adjacency_list_[to_vertex_id].insert(edge_id);
}

}  // namespace uni_course_cpp
