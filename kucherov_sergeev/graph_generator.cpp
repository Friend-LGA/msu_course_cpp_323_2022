#include <cassert>
#include <random>

#include "graph.hpp"
#include "graph_generator.hpp"

namespace uni_course_cpp {
namespace {
static constexpr float kEdgeGreenProbability = 0.1;
static constexpr float kEdgeRedProbability = 0.33;
static constexpr Graph::Depth kYellowEdgeLength = 1;
static constexpr Graph::Depth kRedEdgeLength = 2;

bool get_random_bool(float true_probability) {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::bernoulli_distribution bernoulli_distribution(true_probability);
  return bernoulli_distribution(generator);
}

std::vector<Graph::VertexId> get_unconnected_vertex_ids(
    const Graph& graph,
    Graph::VertexId vertex_id) {
  std::vector<Graph::VertexId> unconnected_vertex_ids = {};
  for (const auto next_depth_vertex_id :
       graph.get_depth_vertex_ids(graph.get_vertex_depth(vertex_id) + 1)) {
    if (graph.is_vertices_connected(vertex_id, next_depth_vertex_id) == false) {
      unconnected_vertex_ids.push_back(next_depth_vertex_id);
    }
  }

  return unconnected_vertex_ids;
}

Graph::VertexId get_random_vertex_id(
    const std::vector<Graph::VertexId>& vertex_ids) {
  assert((vertex_ids.empty() == false) &&
         "Can't pick random vertex id from empty list");

  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<> uniform_int_distribution(
      0, vertex_ids.size() - 1);

  return vertex_ids[uniform_int_distribution(generator)];
}

void generate_green_edges(Graph& graph) {
  for (Graph::Depth current_depth = kGraphDefaultDepth;
       current_depth <= graph.get_depth(); current_depth++) {
    for (const auto vertex_id : graph.get_depth_vertex_ids(current_depth)) {
      if (get_random_bool(kEdgeGreenProbability)) {
        graph.add_edge(vertex_id, vertex_id);
      }
    }
  }
}

void generate_yellow_edges(Graph& graph) {
  const auto graph_depth = graph.get_depth();

  for (Graph::Depth current_depth = kGraphDefaultDepth;
       current_depth <= graph_depth - kYellowEdgeLength; current_depth++) {
    float new_edge_probability = current_depth / (graph_depth - 1.f);

    for (const auto vertex_id : graph.get_depth_vertex_ids(current_depth)) {
      if (get_random_bool(new_edge_probability)) {
        const auto& to_vertex_ids =
            get_unconnected_vertex_ids(graph, vertex_id);

        if (to_vertex_ids.empty() == false) {
          const auto to_vertex_id = get_random_vertex_id(to_vertex_ids);
          graph.add_edge(vertex_id, to_vertex_id);
        }
      }
    }
  }
}

void generate_red_edges(Graph& graph) {
  for (Graph::Depth current_depth = kGraphDefaultDepth;
       current_depth <= graph.get_depth() - kRedEdgeLength; current_depth++) {
    const auto& to_vertex_ids =
        graph.get_depth_vertex_ids(current_depth + kRedEdgeLength);

    if (to_vertex_ids.empty()) {
      break;
    }

    for (const auto vertex_id : graph.get_depth_vertex_ids(current_depth)) {
      if (get_random_bool(kEdgeRedProbability)) {
        const auto to_vertex_id = get_random_vertex_id(to_vertex_ids);
        graph.add_edge(vertex_id, to_vertex_id);
      }
    }
  }
}
}  // namespace

Graph GraphGenerator::generate() const {
  auto graph = Graph();

  if (params_.depth() != 0) {
    graph.add_vertex();
    generate_grey_edges(graph);
    generate_green_edges(graph);
    generate_yellow_edges(graph);
    generate_red_edges(graph);
  }

  return graph;
}

void GraphGenerator::generate_grey_edges(Graph& graph) const {
  for (Graph::Depth current_depth = kGraphDefaultDepth;
       current_depth < params_.depth(); current_depth++) {
    float new_vertex_probability =
        1.f - (current_depth - 1.f) / (params_.depth() - 1.f);

    for (const auto vertex_id : graph.get_depth_vertex_ids(current_depth)) {
      for (int attempt = 0; attempt < params_.new_vertices_count(); attempt++) {
        if (get_random_bool(new_vertex_probability)) {
          const auto new_vertex_id = graph.add_vertex();
          graph.add_edge(vertex_id, new_vertex_id);
        }
      }
    }
  }
}
}  // namespace uni_course_cpp
