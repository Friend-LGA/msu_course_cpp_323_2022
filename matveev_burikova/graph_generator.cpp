#include "graph_generator.hpp"

#include <random>

namespace {
using Graph = uni_course_cpp::Graph;
using GraphGenerator = uni_course_cpp::GraphGenerator;

const std::vector<Graph::VertexId> get_unconnected_vertex_ids(
    Graph graph,
    Graph::VertexId vertex_id,
    const std::vector<Graph::VertexId>& vertex_ids_on_depth) {
  auto unconnected_vertex_ids = vertex_ids_on_depth;
  for (auto vertex_id_iterator = unconnected_vertex_ids.begin();
       vertex_id_iterator != unconnected_vertex_ids.end();) {
    if (graph.has_edge(vertex_id, *vertex_id_iterator))
      unconnected_vertex_ids.erase(vertex_id_iterator);
    else
      ++vertex_id_iterator;
  }
  return unconnected_vertex_ids;
}
bool check_probability(double probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution d(probability);
  return d(gen);
}

Graph::VertexId get_random_vertex_id(
    const std::vector<Graph::VertexId>& vertex_ids_list) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, vertex_ids_list.size() - 1);
  return vertex_ids_list[distrib(gen)];
}

void generate_green_edges(Graph& graph) {
  for (const auto& [vertex_id, vertex] : graph.get_vertices())
    if (check_probability(0.1))
      graph.add_edge(vertex.id(), vertex.id());
}

void generate_yellow_edges(Graph& graph) {
  const Graph::Depth depth = graph.depth();
  const float step = 1.0 / (depth - 2);
  for (Graph::Depth current_depth = 2; current_depth < depth; ++current_depth) {
    const auto& vertex_ids_on_current_depth =
        graph.get_vertex_ids_on_depth(current_depth);
    for (const auto from_vertex_id : vertex_ids_on_current_depth) {
      if (check_probability(step * (current_depth - 1))) {
        const std::vector<Graph::VertexId> not_connected_vertex_ids =
            get_unconnected_vertex_ids(
                graph, from_vertex_id,
                graph.get_vertex_ids_on_depth(current_depth + 1));
        if (not_connected_vertex_ids.size() == 0)
          continue;
        const auto to_vertex_id =
            get_random_vertex_id(not_connected_vertex_ids);
        graph.add_edge(from_vertex_id, to_vertex_id);
      }
    }
  }
}

void generate_red_edges(Graph& graph) {
  const Graph::Depth depth = graph.depth();
  constexpr float probability = 1.0 / 3;
  for (Graph::Depth current_depth = 1; current_depth < depth - 1;
       ++current_depth) {
    const auto& vertex_ids_on_current_depth =
        graph.get_vertex_ids_on_depth(current_depth);
    const auto& vertex_ids_on_following_depth =
        graph.get_vertex_ids_on_depth(current_depth + 2);

    for (const auto from_vertex_id : vertex_ids_on_current_depth) {
      const auto to_vertex_id =
          get_random_vertex_id(vertex_ids_on_following_depth);
      if (check_probability(probability)) {
        graph.add_edge(from_vertex_id, to_vertex_id);
      }
    }
  }
}
}  // namespace

namespace uni_course_cpp {
void GraphGenerator::generate_grey_edges(Graph& graph) const {
  const Graph::Depth depth = params_.depth();
  const float step = 1.0 / (depth - 1);
  for (Graph::Depth current_depth = 0; current_depth < depth; ++current_depth) {
    if (graph.depth() < current_depth + 1)
      break;
    for (const auto vertex_id :
         graph.get_vertex_ids_on_depth(current_depth + 1))
      for (int j = 0; j < params_.new_vertices_count(); ++j) {
        if (check_probability(1 - current_depth * step)) {
          const auto new_vertex_id = graph.add_vertex();
          graph.add_edge(vertex_id, new_vertex_id);
        }
      }
  }
}

Graph GraphGenerator::generate() const {
  auto graph = Graph();
  if (params_.depth() == 0)
    return graph;
  graph.add_vertex();
  if (params_.new_vertices_count() == 0)
    return graph;
  generate_grey_edges(graph);
  generate_green_edges(graph);
  generate_yellow_edges(graph);
  generate_red_edges(graph);
  return graph;
}
}  // namespace uni_course_cpp
