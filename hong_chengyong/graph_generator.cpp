#include "graph_generator.hpp"
#include <iostream>
#include <random>
#include <vector>
#include "graph.hpp"

namespace uni_course_cpp {
constexpr float GREEN_PROBABILITY = 0.1, RED_PROBABILITY = 0.33;
float getGreyProbability(float step, int depth) {
  return 1.0 - step * depth;
}
float getYellowProbability(const uni_course_cpp::Graph& graph,
                           const uni_course_cpp::VertexId& vertex_id) {
  return 1.0 * graph.vertexDepth(vertex_id) / (graph.depth() - 1);
}
bool randomValue(float probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution distribution(probability);
  return distribution(gen);
}
int getRandomVertexId(const std::vector<uni_course_cpp::VertexId>& vertex_ids) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> random_number(0, vertex_ids.size() - 1);
  return vertex_ids.at(random_number(gen));
}
}  // namespace uni_course_cpp

namespace uni_course_cpp {

Graph GraphGenerator::generateMainBody() const {
  Graph graph;
  float step = 1.0 / params_.depth;
  graph.addVertex();
  for (int current_depth = 0; current_depth < params_.depth; current_depth++) {
    bool vertexes_generated = false;
    const auto previous_layer =
        graph.vertexIdsAtLayer(current_depth);  // catch request addVertex()
    for (const auto& vertex_id : previous_layer) {
      for (int j = 0; j < params_.new_vertexes_num; j++) {
        if (randomValue(getGreyProbability(step, current_depth))) {
          graph.addEdge(vertex_id, graph.addVertex());
          vertexes_generated = true;
        }
      }
    }
    if (!vertexes_generated) {
      break;
    }
  }
  if (graph.depth() < params_.depth) {
    std::cout << "Depth of the graph is less than given. Depth is "
              << graph.depth() << std::endl;
  }
  return graph;
}

void GraphGenerator::generateColorEdges(Graph& graph) const {
  for (const auto& vertex : graph.vertexes()) {
    int vertex_depth = graph.vertexDepth(vertex.id);
    if (randomValue(GREEN_PROBABILITY)) {
      graph.addEdge(vertex.id, vertex.id);
    }
    const auto& next_vertex_id = vertex.id + 1;
    if (vertex_depth < graph.depth() &&
        randomValue(getYellowProbability(graph, vertex.id))) {
      std::vector<VertexId> next_layer;
      for (const auto& vertex_id : graph.vertexIdsAtLayer(vertex_depth + 1)) {
        if (!graph.is_connected(vertex.id, vertex_id)) {
          next_layer.push_back(vertex_id);
        }
      }
      graph.addEdge(vertex.id, getRandomVertexId(next_layer));
    }
    if (randomValue(RED_PROBABILITY) && vertex_depth < (graph.depth() - 1)) {
      graph.addEdge(vertex.id, getRandomVertexId(
                                   graph.vertexIdsAtLayer(vertex_depth + 2)));
    }
  }
}

Graph GraphGenerator::generate() const {
  Graph graph = generateMainBody();
  generateColorEdges(graph);
  return graph;
}
}  // namespace uni_course_cpp
