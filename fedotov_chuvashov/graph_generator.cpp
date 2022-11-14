#include "graph_generator.hpp"
#include <algorithm>
#include <random>

namespace uni_course_cpp {

namespace {
bool check_probabilty(float probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution p(probability);

  return p(gen);
}

int get_random_vertex_id(const std::set<Graph::VertexId>& vertex_ids) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, vertex_ids.size() - 1);
  return *std::next(vertex_ids.begin(), distrib(gen));
}

void generate_green_edges(Graph& graph) {
  constexpr float probabilty_of_green_edge = 0.1;
  for (const auto& [vertex_id, _] : graph.vertices()) {
    if (check_probabilty(probabilty_of_green_edge)) {
      graph.add_edge(vertex_id, vertex_id);
    }
  }
}

void generate_red_edges(Graph& graph) {
  constexpr float probabilty_of_red_edge = 0.33;
  for (Graph::Depth current_depth = 1;
       current_depth <= graph.depth() - kRedDepthDifference; ++current_depth) {
    for (auto vertex_id : graph.vertices_at_depth(current_depth)) {
      if (check_probabilty(probabilty_of_red_edge)) {
        const std::set<Graph::VertexId>& deeper_vertices =
            graph.vertices_at_depth(current_depth + kRedDepthDifference);
        if (deeper_vertices.size() > 0) {
          const Graph::VertexId random_vertex_id =
              get_random_vertex_id(deeper_vertices);
          graph.add_edge(vertex_id, random_vertex_id);
        }
      }
    }
  }
}

std::set<Graph::VertexId> get_unconnected_vertex_ids(
    const Graph& graph,
    Graph::Depth current_depth,
    Graph::VertexId vertex_id) {
  const std::set<Graph::VertexId>& deeper_vertices =
      graph.vertices_at_depth(current_depth + kYellowDepthDifference);
  const std::set<Graph::VertexId> children_vertices =
      graph.children_of_vertex(vertex_id);
  std::set<Graph::VertexId> unconnected_vertices;
  std::set_difference(
      deeper_vertices.begin(), deeper_vertices.end(), children_vertices.begin(),
      children_vertices.end(),
      std::inserter(unconnected_vertices, unconnected_vertices.begin()));
  return unconnected_vertices;
}

void generate_yellow_edges(Graph& graph) {
  for (Graph::Depth current_depth = 1;
       current_depth <= graph.depth() - kYellowDepthDifference;
       ++current_depth) {
    for (auto vertex_id : graph.vertices_at_depth(current_depth)) {
      const float probabilty_of_yellow_edge =
          static_cast<float>((current_depth - kYellowDepthDifference)) /
          (graph.depth() - kYellowDepthDifference - kGraphBaseDepth);
      if (check_probabilty(probabilty_of_yellow_edge)) {
        std::set<Graph::VertexId> unconnected_vertices =
            get_unconnected_vertex_ids(graph, current_depth, vertex_id);
        if (unconnected_vertices.size() > 0) {
          const Graph::VertexId random_vertex_id =
              get_random_vertex_id(unconnected_vertices);
          graph.add_edge(vertex_id, random_vertex_id);
        }
      }
    }
  }
}

}  // namespace

void GraphGenerator::generate_new_vertices(Graph& graph,
                                           Graph::VertexId root_id,
                                           Graph::Depth depth) const {
  if (depth <= 0) {
    return;
  }
  const float probabilty_of_grey_edge =
      1.0 - static_cast<float>(
                (graph.get_vertex_depth(root_id) - kGreyDepthDifference)) /
                (params_.depth() - kGreyDepthDifference);
  for (int i = 0; i < params_.new_vertices_count(); ++i) {
    if (check_probabilty(probabilty_of_grey_edge)) {
      const Graph::VertexId child_id = graph.add_vertex();
      graph.add_edge(root_id, child_id);
      generate_new_vertices(graph, child_id, depth - 1);
    }
  }
}

Graph GraphGenerator::generate() const {
  auto graph = Graph();
  if (params_.depth() > 0) {
    const Graph::VertexId root_id = graph.add_vertex();
    generate_new_vertices(graph, root_id, params_.depth());
  }
  generate_green_edges(graph);
  generate_red_edges(graph);
  generate_yellow_edges(graph);
  return graph;
}
}  // namespace uni_course_cpp
