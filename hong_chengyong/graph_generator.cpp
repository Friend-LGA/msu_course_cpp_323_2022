#include "graph_generator.hpp"
#include <iostream>
#include <random>
#include <vector>
#include "graph.hpp"

namespace
{
  constexpr float kGreenProbability = 0.1, kBlueProbability = 0.25,
                  kRedProbability = 0.33;
  float getGreyProbability(float step, int depth)
  {
    return 1.0 - step * depth;
  }
  float getYellowProbability(const uni_course_cpp::Graph &graph,
                             const uni_course_cpp::VertexId &vertex_id)
  {
    return 1.0 * graph.vertexDepth(vertex_id) / (graph.depth() - 1);
  }
  bool randomValue(float probability)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution distribution(probability);
    return distribution(gen);
  }
  int getRandomVertexId(const std::vector<uni_course_cpp::VertexId> &vertex_ids)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> random_number(0, vertex_ids.size() - 1);
    return vertex_ids.at(random_number(gen));
  }
} // namespace

namespace uni_course_cpp
{

  Graph GraphGenerator::generateMainBody() const
  {
    Graph graph;
    float step = 1.0 / params_.depth;
    graph.addVertex();
    for (int current_depth = 0; current_depth < params_.depth; current_depth++)
    {
      bool vertexes_generated = false;
      const auto previous_layer = graph.vertexIdsAtLayer(
          current_depth);
      for (const auto &vertex_id : previous_layer)
      {
        for (int j = 0; j < params_.new_vertexes_num; j++)
        {
          if (randomValue(getGreyProbability(step, current_depth)))
          {
            graph.addEdge(vertex_id, graph.addVertex());
            vertexes_generated = true;
          }
        }
      }
      if (!vertexes_generated)
      {
        break;
      }
    }
    if (graph.depth() < params_.depth)
    {
      std::cout << "Depth of the graph is less than given. Depth is "
                << graph.depth() << std::endl;
    }
    return graph;
  }
  void GraphGenerator::generate_green_edges(Graph &graph,
                                            const Depth &vertex_depth,
                                            const Vertex &vertex) const
  {
    if (randomValue(kGreenProbability))
    {
      graph.addEdge(vertex.id, vertex.id);
    }
  }
  void GraphGenerator::generate_blue_edges(Graph &graph,
                                           const Depth &vertex_depth,
                                           const Vertex &vertex) const
  {
    const auto &next_vertex_id = vertex.id + 1;
    if (randomValue(kBlueProbability) && graph.hasVertex(next_vertex_id) &&
        graph.vertexDepth(next_vertex_id) == vertex_depth)
    {
      graph.addEdge(vertex.id, next_vertex_id);
    }
  }
  void GraphGenerator::generate_yellow_edges(Graph &graph,
                                             const Depth &vertex_depth,
                                             const Vertex &vertex) const
  {
    if (vertex_depth < graph.depth() &&
        randomValue(getYellowProbability(graph, vertex.id)))
    {
      std::vector<VertexId> next_layer;
      for (const auto &vertex_id : graph.vertexIdsAtLayer(
               vertex_depth + 1)) // find vertex from next layer
      {
        if (!graph.areConnected(vertex.id, vertex_id))
        {
          next_layer.push_back(vertex_id);
        }
      }
      if (next_layer.size() != 0)
        graph.addEdge(vertex.id, getRandomVertexId(next_layer));
    }
  }
  void GraphGenerator::generate_red_edges(Graph &graph,
                                          const Depth &vertex_depth,
                                          const Vertex &vertex) const
  {
    if (randomValue(kRedProbability) &&
        vertex_depth < (graph.depth() - 1)) // depth(N-1){
      graph.addEdge(vertex.id,
                    getRandomVertexId(graph.vertexIdsAtLayer(
                        vertex_depth + 2))); // fin vertex from next next layer
  }

  void GraphGenerator::generateColorEdges(Graph &graph) const
  {
    for (const auto &vertex : graph.vertexes())
    {
      int vertex_depth = graph.vertexDepth(vertex.id);
      generate_green_edges(graph, vertex_depth, vertex);
      generate_blue_edges(graph, vertex_depth, vertex);
      generate_yellow_edges(graph, vertex_depth, vertex);
      generate_red_edges(graph, vertex_depth, vertex);
    }
  }

  Graph GraphGenerator::generate() const
  {
    Graph graph = generateMainBody();
    generateColorEdges(graph);
    return graph;
  }
} // namespace uni_course_cpp
