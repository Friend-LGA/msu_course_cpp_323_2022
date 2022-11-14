#include "graph_generator.hpp"
#include <random>

namespace uni_course_cpp {

namespace {
std::vector<Graph::VertexId> get_unconnected_vertex_ids(
    const Graph& graph,
    Graph::VertexId vertex_id,
    const std::vector<Graph::VertexId>& vertex_ids_on_depth) {
  std::vector<Graph::VertexId> unconnected_vertices = {};
  for (const auto cur_vertex_id : vertex_ids_on_depth) {
    if (!graph.is_connected(vertex_id, cur_vertex_id)) {
      unconnected_vertices.emplace_back(cur_vertex_id);
    }
  }

  return unconnected_vertices;
}

int get_random_vertex(const std::vector<Graph::VertexId>& vertex_ids) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, vertex_ids.size() - 1);
  return vertex_ids[dis(gen)];
}

bool check_probability(float prob) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution d(prob);
  return d(gen);
}

void generate_green_edges(Graph& graph) {
  static float constexpr prob = 0.1;
  const auto& vertices = graph.vertices();
  for (const auto& [vertex_id, vertex] : vertices) {
    if (check_probability(prob)) {
      graph.add_edge(vertex.id(), vertex.id());
    }
  }
}

void generate_yellow_edges(Graph& graph, Graph::Depth depth) {
  float step = 1.0 / (depth - 2);
  for (Graph::Depth cur_depth = 2; cur_depth <= depth - 1; cur_depth++) {
    float prob = (float)(step * (cur_depth - 1));

    for (const auto from_vertex_id : graph.vertices_of_depth(cur_depth)) {
      if (check_probability(prob)) {
        const auto unconnected_vertices_ids = get_unconnected_vertex_ids(
            graph, from_vertex_id, graph.vertices_of_depth(cur_depth + 1));
        if (!unconnected_vertices_ids.empty()) {
          const auto to_vertex_id = get_random_vertex(unconnected_vertices_ids);
          graph.add_edge(from_vertex_id, to_vertex_id);
        }
      }
    }
  }
}

void generate_red_edges(Graph& graph, Graph::Depth depth) {
  static float constexpr prob = (float)1 / 3;
  for (Graph::Depth cur_depth = 1; cur_depth <= depth - 2; cur_depth++) {
    const auto& next_vertices_depth = graph.vertices_of_depth(cur_depth + 2);

    for (const auto from_vertex_id : graph.vertices_of_depth(cur_depth)) {
      const auto to_vertex_id = get_random_vertex(next_vertices_depth);
      if (check_probability(prob)) {
        graph.add_edge(from_vertex_id, to_vertex_id);
      }
    }
  }
}

}  // namespace

Graph GraphGenerator::generate() const {
  auto graph = Graph();
  if (params_.depth()) {
    graph.add_vertex();
    generate_grey_edges(graph);
    generate_green_edges(graph);
    generate_yellow_edges(graph, graph.get_graph_depth());
    generate_red_edges(graph, graph.get_graph_depth());
  }
  return graph;
}

void GraphGenerator::generate_grey_edges(Graph& graph) const {
  const auto depth = params_.depth();
  const auto new_vertices_count = params_.new_vertices_count();
  float step = 1.0 / (depth - 1);

  for (Graph::Depth cur_depth = 1; cur_depth <= depth; cur_depth++) {
    float prob = (float)(1 - step * (cur_depth - 1));
    bool is_created = false;
    for (const auto from_vertex_id : graph.vertices_of_depth(cur_depth)) {
      for (Graph::VertexId cur_vertex_count = 0;
           cur_vertex_count < new_vertices_count; cur_vertex_count++) {
        if (check_probability(prob)) {
          is_created = true;
          auto vertex_id = graph.add_vertex();
          graph.add_edge(from_vertex_id, vertex_id);
        }
      }
    }

    if (!is_created)
      break;
  }
}

}  // namespace uni_course_cpp
