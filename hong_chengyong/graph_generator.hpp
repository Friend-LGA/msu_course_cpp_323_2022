#include <random>
#include <vector>
#include "graph.hpp"

namespace uni_course_cpp {
constexpr float kGreenProbabilty = 0.1, kBlueProbabilty = 0.25,
                kRedProbability = 0.33;

class GraphGenerator {
 public:
  struct Params {
    explicit Params(int _depth = 0, int _new_vertexes_num = 0)
        : depth(_depth), new_vertexes_num(_new_vertexes_num) {}

    const Depth depth;
    const int new_vertexes_num;
  };

  explicit GraphGenerator(const Params& params = Params()) : params_(params) {}

  Graph generateMainBody() const {
    Graph graph;
    float step = 1.0 / params_.depth;
    graph.addVertex();
    for (int current_depth = 0; current_depth < params_.depth;
         current_depth++) {
      bool vertexes_generated = false;
      const auto previous_layer =
          graph.vertexIdsAtLayer(current_depth);  // 上一层=现在深度层
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
  void generate_green_edges(Graph& graph,
                            const Depth& vertex_depth,
                            const Vertex& vertex) const {
    if (randomValue(kGreenProbabilty)) {
      graph.addEdge(vertex.id, vertex.id);
    }
  }
  void generate_blue_edges(Graph& graph,
                           const Depth& vertex_depth,
                           const Vertex& vertex) const {
    const auto& next_vertex_id = vertex.id + 1;
    if (randomValue(kBlueProbabilty) && graph.hasVertex(next_vertex_id) &&
        graph.vertexDepth(next_vertex_id) == vertex_depth) {
      graph.addEdge(vertex.id, next_vertex_id);
    }
  }
  void generate_yellow_edges(Graph& graph,
                             const Depth& vertex_depth,
                             const Vertex& vertex) const {
    if (vertex_depth < graph.depth() &&
        randomValue(getYellowProbability(graph, vertex.id))) {
      std::vector<VertexId> next_layer;
      for (const auto& vertex_id : graph.vertexIdsAtLayer(
               vertex_depth + 1))  // find vertex from next layer
      {
        if (!graph.areConnected(vertex.id, vertex_id)) {
          next_layer.push_back(vertex_id);
        }
      }
      if (next_layer.size() != 0)
        graph.addEdge(vertex.id, getRandomVertexId(next_layer));
    }
  }
  void generate_red_edges(Graph& graph,
                          const Depth& vertex_depth,
                          const Vertex& vertex) const {
    if (randomValue(kRedProbability) &&
        vertex_depth < (graph.depth() - 1))  // depth(N-1){
      graph.addEdge(vertex.id,
                    getRandomVertexId(graph.vertexIdsAtLayer(
                        vertex_depth + 2)));  // fin vertex from next next layer
  }

  void generateColorEdges(Graph& graph) const {
    for (const auto& vertex : graph.vertexes()) {
      const int vertex_depth = graph.vertexDepth(vertex.id);
      generate_green_edges(graph, vertex_depth, vertex);
      generate_blue_edges(graph, vertex_depth, vertex);
      generate_yellow_edges(graph, vertex_depth, vertex);
      generate_red_edges(graph, vertex_depth, vertex);
    }
  }

  Graph generate() const {
    Graph graph = generateMainBody();
    generateColorEdges(graph);
    return graph;
  }

 private:
  const Params params_ = Params();
  bool randomValue(float probability) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution distribution(probability);
    return distribution(gen);
  }
  int getRandomVertexId(const std::vector<VertexId> vertex_ids) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> random_number(0, vertex_ids.size() - 1);
    return vertex_ids.at(random_number(gen));
  }
  float getGreyProbability(float step, int depth) const {
    return 1.0 - step * depth;
  }
  float getYellowProbability(const Graph& graph,
                             const VertexId& vertex_id) const {
    return 1.0 * graph.vertexDepth(vertex_id) / (graph.depth() - 1);
  }
};
}  // namespace uni_course_cpp
