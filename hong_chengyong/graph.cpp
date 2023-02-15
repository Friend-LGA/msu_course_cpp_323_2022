#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using VertexId = int;
using EdgeId = int;
using Depth = int;

struct Vertex {
  const VertexId id;

  explicit Vertex(const VertexId& _id) : id(_id) {}
};

struct Edge {
  enum class Colors { Grey, Green, Blue, Yellow, Red };

  const EdgeId id;
  const VertexId from_vertex_id;
  const VertexId to_vertex_id;
  const Colors color;
  explicit Edge(const EdgeId& _id,
                const VertexId& _from_vertex_id,
                const VertexId& _to_vertex_id,
                const Colors& _color)
      : id(_id),
        from_vertex_id(_from_vertex_id),
        to_vertex_id(_to_vertex_id),
        color(_color) {}
};

class Graph {
 public:
  bool hasVertex(const VertexId& vertex_id) const {
    for (const auto& vertex : vertexes_) {
      if (vertex.id == vertex_id) {
        return true;
      }
    }
    return false;
  }

  bool hasEdge(const EdgeId& edge_id) const {
    for (const auto& edge : edges_) {
      if (edge.id == edge_id) {
        return true;
      }
    }
    return false;
  }

  bool areConnected(const VertexId& from_vertex_id,
                    const VertexId& to_vertex_id) const {
    assert(hasVertex(from_vertex_id) && "Vertex1 index is out of range");
    assert(hasVertex(to_vertex_id) && "Vertex2 index is out of range");
    if (from_vertex_id == to_vertex_id) {
      for (const auto& edge_id : connection_list_.at(from_vertex_id)) {
        const auto& edge = getEdge(edge_id);
        if (edge.from_vertex_id == from_vertex_id &&
            edge.to_vertex_id == from_vertex_id) {
          return true;
        }
      }
    } else {
      for (const auto& edge_id : connection_list_.at(from_vertex_id)) {
        const auto& edge = getEdge(edge_id);
        if (edge.from_vertex_id == to_vertex_id ||
            edge.to_vertex_id == to_vertex_id) {
          return true;
        }
      }
    }
    return false;
  }

  VertexId addVertex() {
    const auto& new_vertex = vertexes_.emplace_back(getNewVertexId());
    connection_list_.insert({new_vertex.id, std::vector<EdgeId>()});
    layers_list_[0].push_back(new_vertex.id);
    vertexes_depths_[new_vertex.id] = 0;
    return new_vertex.id;
  }

  Edge::Colors calculateEdgeColor(const VertexId& from_vertex_id,
                                  const VertexId& to_vertex_id) const {
    if (connection_list_.at(to_vertex_id).size() == 0 ||
        connection_list_.at(from_vertex_id).size() == 0) {
      return Edge::Colors::Grey;
    } else if (from_vertex_id == to_vertex_id) {
      return Edge::Colors::Green;
    } else if (vertexDepth(to_vertex_id) == vertexDepth(from_vertex_id)) {
      return Edge::Colors::Blue;
    } else if (std::abs(vertexDepth(to_vertex_id) -
                        vertexDepth(from_vertex_id)) == 1) {
      return Edge::Colors::Yellow;
    } else {
      return Edge::Colors::Red;
    }
  }

  void greyEdgeInitialization(const VertexId& from_vertex_id,
                              const VertexId& to_vertex_id) {
    int new_depth =
        vertexes_depths_[std::min(from_vertex_id, to_vertex_id)] + 1;
    vertexes_depths_[std::max(from_vertex_id, to_vertex_id)] = new_depth;
    depth_ = std::max(depth_, new_depth);
    layers_list_[new_depth].push_back(std::max(from_vertex_id, to_vertex_id));
    for (auto it = layers_list_[0].begin(); it != layers_list_[0].end(); it++) {
      if (*it == std::max(from_vertex_id, to_vertex_id)) {
        layers_list_[0].erase(it);
        break;
      }
    }
  }

  void addEdge(const VertexId& from_vertex_id, const VertexId& to_vertex_id) {
    assert(hasVertex(from_vertex_id) && "Vertex1 index is out of range");
    assert(hasVertex(to_vertex_id) && "Vertex2 index is out of range");
    assert(!areConnected(from_vertex_id, to_vertex_id) &&
           "These vertexes are already connected");
    auto color = calculateEdgeColor(from_vertex_id, to_vertex_id);
    if (color == Edge::Colors::Grey) {
      greyEdgeInitialization(from_vertex_id, to_vertex_id);
    }
    const auto& new_edge = edges_.emplace_back(getNewEdgeId(), from_vertex_id,
                                               to_vertex_id, color);
    connection_list_[from_vertex_id].push_back(new_edge.id);
    if (from_vertex_id != to_vertex_id) {
      connection_list_[to_vertex_id].push_back(new_edge.id);
    }
  }

  const Edge& getEdge(const EdgeId& edge_id) const {
    assert(hasEdge(edge_id) && "Edge id is out of range.");
    for (const auto& edge : edges_) {
      if (edge.id == edge_id) {
        return edge;
      }
    }
    throw std::runtime_error("Cannot be reached.");
  }

  const std::vector<EdgeId>& vertexConnections(const VertexId& id) const {
    assert(hasVertex(id) && "Vertex id is out of range");
    return connection_list_.at(id);
  }
  const std::vector<Vertex>& vertexes() const { return vertexes_; }
  const std::vector<Edge>& edges() const { return edges_; }
  const std::vector<VertexId>& vertexIdsAtLayer(Depth depth) const {
    assert(depth <= depth_ && "Graph is not that deep");
    return layers_list_.at(depth);
  }
  int vertexDepth(const VertexId& vertex_id) const {
    assert(hasVertex(vertex_id) && "Vertex id is out of range");
    return vertexes_depths_.at(vertex_id);
  }
  int depth() const { return depth_; }

 private:
  std::vector<Vertex> vertexes_;
  std::vector<Edge> edges_;
  Depth depth_ = 0;
  int vertex_new_id_ = 0, edge_new_id_ = 0;
  std::unordered_map<VertexId, std::vector<EdgeId>> connection_list_;
  std::unordered_map<int, std::vector<VertexId>> layers_list_;
  std::unordered_map<VertexId, int> vertexes_depths_;
  VertexId getNewVertexId() { return vertex_new_id_++; }
  EdgeId getNewEdgeId() { return edge_new_id_++; }
};

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

class GraphPrinter {
 public:
  explicit GraphPrinter(const Graph& graph) : graph_(graph) {}
  std::string print() const {
    std::string graph_string;
    graph_string += "{\n \"vertices\": [\n  ";
    for (const auto& vertex : graph_.vertexes()) {
      graph_string += printVertex(vertex.id);
    }
    if (graph_.vertexes().size() > 0) {
      graph_string.pop_back();
      graph_string.pop_back();
    }
    graph_string += "\n ],\n \"edges\": [\n  ";
    for (const auto& edge : graph_.edges()) {
      graph_string += printEdge(edge);
    }
    if (graph_.edges().size() > 0) {
      graph_string.pop_back();
      graph_string.pop_back();
    }
    graph_string += "\n ]\n}\n";
    return graph_string;
  }
  std::string printVertex(const VertexId& id) const {
    std::string vertex_string =
        "{\n   \"id\": " + std::to_string(id) + ",\n   \"edge_ids\": [";
    for (const auto& edge_id : graph_.vertexConnections(id)) {
      vertex_string += std::to_string(edge_id) + ", ";
    }
    if (graph_.vertexConnections(id).size() > 0) {
      vertex_string.pop_back();
      vertex_string.pop_back();
    }
    vertex_string +=
        "],\n   \"depth\": " + std::to_string(graph_.vertexDepth(id)) +
        "\n  }, ";
    return vertex_string;
  }
  std::string printEdge(const Edge& edge) const {
    std::string edge_string = "{\n   \"id\": " + std::to_string(edge.id);
    edge_string += ",\n   \"vertex_ids\": [";
    edge_string += std::to_string(edge.from_vertex_id) + ", ";
    edge_string += std::to_string(edge.to_vertex_id) + "],\n   ";
    edge_string += "\"color\": " + colorToString(edge) + "\n  }, ";
    return edge_string;
  }

 private:
  const Graph& graph_;
  std::string colorToString(const Edge& edge) const {
    switch (edge.color) {
      case Edge::Colors::Grey:
        return "\"gray\"";
      case Edge::Colors::Green:
        return "\"green\"";
      case Edge::Colors::Blue:
        return "\"blue\"";
      case Edge::Colors::Yellow:
        return "\"yellow\"";
      case Edge::Colors::Red:
        return "\"red\"";
    }
  }
};

void write_to_file(const std::string& string, const std::string& file_name) {
  std::ofstream file(file_name);
  file << string;
  file.close();
}

int main() {
  Depth depth, new_vertexes_num;
  std::cout << "Enter depth:" << std::endl;
  std::cin >> depth;
  while (depth < 0) {
    std::cout << "Depth must be not negative. Enter depth again:" << std::endl;
    std::cin >> depth;
  }
  std::cout << "Enter number of new vertices:" << std::endl;
  std::cin >> new_vertexes_num;
  while (depth < 0) {
    std::cout << "Number of new vertices must be not negative. Enter number of "
                 "new vertexes again:"
              << std::endl;
    std::cin >> new_vertexes_num;
  }
  const GraphGenerator::Params params =
      GraphGenerator::Params(depth, new_vertexes_num);
  const Graph graph = GraphGenerator(params).generate();
  const auto graph_printer = GraphPrinter(graph);
  const auto graph_json = graph_printer.print();
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");
  return 0;
}
