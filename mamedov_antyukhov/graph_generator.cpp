#include "graph_generator.hpp"
#include <iostream>
#include <random>

bool check_probability(double probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution d(probability);
  return d(gen);
}

Graph::VertexId get_random_vertex_id(std::size_t size) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, size);
  return distrib(gen);
}

std::vector<Graph::VertexId> get_unconnected_vertex_ids(
    const std::vector<Graph::VertexId>& vertices,
    const Graph& graph,
    const Graph::VertexId vertex) {
  std::vector<Graph::VertexId> result;
  for (const auto cur_vertex : vertices) {
    if (!graph.has_edge(vertex, cur_vertex)) {
      result.emplace_back(cur_vertex);
    }
  }
  return result;
}

Graph GraphGenerator::generate() const {
  auto graph = Graph();
  if (!params_.depth()) {
    return graph;
  }
  graph.add_vertex();
  generate_grey_edges(graph);
  generate_green_edges(graph);
  generate_yellow_edges(graph);
  generate_red_edges(graph);
  return graph;
}

void GraphGenerator::generate_grey_edges(Graph& graph) const {
  const auto max_depth = params_.depth() - 1;
  const auto new_vertices_count = params_.new_vertices_count();

  for (Graph::Depth current_depth = 0;
       current_depth < max_depth && current_depth < graph.depth();
       ++current_depth) {
    for (const auto vertex : graph.get_vertex_ids_at_depth(current_depth)) {
      for (int i = 0; i < new_vertices_count; ++i) {
        if (check_probability(1.0 - (1.0 / max_depth) * current_depth)) {
          graph.add_edge(vertex, graph.add_vertex());
        }
      }
    }
  }
}

void GraphGenerator::generate_green_edges(Graph& graph) const {
  const auto& vertices = graph.get_vertices();
  for (auto vertex : vertices) {
    if (check_probability(constants::green_edges_probability)) {
      graph.add_edge(vertex.first, vertex.first);
    }
  }
}

void GraphGenerator::generate_yellow_edges(Graph& graph) const {
  const auto max_depth = graph.depth() - 2;

  for (Graph::Depth current_depth = 1; current_depth <= max_depth;
       ++current_depth) {
    for (const auto vertex : graph.get_vertex_ids_at_depth(current_depth)) {
      if (check_probability((1.0 / max_depth) * current_depth)) {
        const auto next_level_vertices = get_unconnected_vertex_ids(
            graph.get_vertex_ids_at_depth(current_depth + 1), graph, vertex);
        if (!next_level_vertices.empty()) {
          graph.add_edge(vertex, next_level_vertices[get_random_vertex_id(
                                     next_level_vertices.size() - 1)]);
        }
      }
    }
  }
}

void GraphGenerator::generate_red_edges(Graph& graph) const {
  const auto max_depth = graph.depth() - 2;

  for (Graph::Depth current_depth = 0; current_depth < max_depth;
       ++current_depth) {
    for (const auto vertex : graph.get_vertex_ids_at_depth(current_depth)) {
      if (check_probability(constants::red_edges_probability)) {
        const auto next_level_vertices =
            graph.get_vertex_ids_at_depth(current_depth + 2);
        graph.add_edge(vertex, next_level_vertices[get_random_vertex_id(
                                   next_level_vertices.size() - 1)]);
      }
    }
  }
}
