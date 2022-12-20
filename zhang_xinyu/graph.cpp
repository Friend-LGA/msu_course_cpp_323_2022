#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class Graph {
 public:
  using Depth = int;
  using VertexId = int;
  using EdgeId = int;

  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }

   private:
    VertexId id_ = 0;
  };

  struct Edge {
   public:
    enum class Color { Grey, Green, Yellow, Red };

    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id, Color color)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id),
          color_(color) {}

    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; }
    VertexId to_vertex_id() const { return to_vertex_id_; }
    Color color() const { return color_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
    Color color_ = Color::Grey;
  };

  bool is_connected(VertexId, VertexId) const;
  VertexId add_vertex();

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

  Depth get_graph_depth() const {
    return vertexes_of_depth_.size() ? vertexes_of_depth_.size() - 1 : 0;
  }
  Depth get_vertex_depth(VertexId id) const;

  const std::unordered_map<VertexId, Vertex>& get_vertexes() const {
    return vertexes_;
  }
  const std::vector<VertexId>& vertexes_of_depth(Depth depth) const {
    return vertexes_of_depth_[depth];
  };
  const std::unordered_map<EdgeId, Edge>& get_edges() const { return edges_; }
  const std::set<EdgeId>& connected_edge_ids(VertexId vertex_id) const {
    return adjacency_list_.at(vertex_id);
  }

  bool has_vertex(VertexId id) const {
    return vertexes_.find(id) != vertexes_.end();
  }

 private:
  EdgeId gen_new_edge_id() { return last_edge_id_++; }
  VertexId gen_new_vertex_id() { return last_vertex_id_++; }

  Edge::Color define_color(VertexId from_vertex_id,
                           VertexId to_vertex_id) const;
  void set_vertex_depth(VertexId id, Depth depth);

  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
  std::unordered_map<VertexId, Depth> depth_of_vertexes_;
  std::vector<std::vector<VertexId>> vertexes_of_depth_;
  std::unordered_map<VertexId, Vertex> vertexes_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, std::set<EdgeId>> adjacency_list_;
};

static constexpr Graph::Depth kBaseDepth = 1;

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
  }

  return color;
}

Graph::VertexId Graph::add_vertex() {
  const auto vertex_id = gen_new_vertex_id();
  vertexes_.insert(std::make_pair(vertex_id, Vertex(vertex_id)));

  if (vertexes_of_depth_.empty()) {
    std::vector<VertexId> empty_vertex = {};
    // add empty vector to keep vertices at depth 0 of the graph
    vertexes_of_depth_.emplace_back(empty_vertex);
    // add empty vector to keep vertices at depth 1 of the graph
    vertexes_of_depth_.emplace_back(empty_vertex);
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
  const auto edge_id = gen_new_vertex_id();
  const auto edge_color = define_color(from_vertex_id, to_vertex_id);

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

class GraphGenerator {
 public:
  struct Params {
   public:
    Params(Graph::Depth depth, int new_vertexes_count)
        : depth_(depth), new_vertexes_count_(new_vertexes_count) {}

    Graph::Depth depth() const { return depth_; }
    int new_vertexes_count() const { return new_vertexes_count_; }

   private:
    Graph::Depth depth_ = 0;
    int new_vertexes_count_ = 0;
  };

  explicit GraphGenerator(Params&& params) : params_(std::move(params)) {}

  Graph generate() const;

 private:
  void generate_grey_edges(Graph& graph) const;
  void generate_green_edges(Graph& graph) const;
  void generate_yellow_edges(Graph& graph) const;
  void generate_red_edges(Graph& graph) const;
  Params params_ = Params(0, 0);
};

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

Graph GraphGenerator::generate() const {
  auto graph = Graph();
  if (params_.depth()) {
    graph.add_vertex();
    generate_grey_edges(graph);
    generate_green_edges(graph);
    generate_yellow_edges(graph);
    generate_red_edges(graph);
  }
  return graph;
}

void GraphGenerator::generate_grey_edges(Graph& graph) const {
  const auto depth = params_.depth();
  const auto new_vertexes_count = params_.new_vertexes_count();
  float step = 1.0 / (depth - 1);

  for (Graph::Depth current_depth = 1; current_depth <= depth;
       current_depth++) {
    float prob = (float)(1 - step * (current_depth - 1));
    bool is_created = false;
    for (const auto from_vertex_id : graph.vertexes_of_depth(current_depth)) {
      for (Graph::VertexId cur_vertex_count = 0;
           cur_vertex_count < new_vertexes_count; cur_vertex_count++) {
        if (check_probability(prob)) {
          is_created = true;
          auto vertex_id = graph.add_vertex();
          graph.add_edge(from_vertex_id, vertex_id);
        }
      }
    }
    if (!is_created) {
      break;
    }
  }
}

void GraphGenerator::generate_green_edges(Graph& graph) const {
  static float constexpr prob = 0.1;
  const auto& vertices = graph.get_vertexes();
  for (const auto& [vertex_id, vertex] : vertices) {
    if (check_probability(prob)) {
      graph.add_edge(vertex.id(), vertex.id());
    }
  }
}

void GraphGenerator::generate_yellow_edges(Graph& graph) const {
  const auto depth = graph.get_graph_depth();
  const float step = 1.0 / (depth - 2);
  for (Graph::Depth current_depth = 2; current_depth <= depth - 1;
       current_depth++) {
    float prob = (float)(step * (current_depth - 1));
    for (const auto from_vertex_id : graph.vertexes_of_depth(current_depth)) {
      if (check_probability(prob)) {
        const auto unconnected_vertexes_ids = get_unconnected_vertex_ids(
            graph, from_vertex_id, graph.vertexes_of_depth(current_depth + 1));
        if (!unconnected_vertexes_ids.empty()) {
          const auto to_vertex_id = get_random_vertex(unconnected_vertexes_ids);
          graph.add_edge(from_vertex_id, to_vertex_id);
        }
      }
    }
  }
}

void GraphGenerator::generate_red_edges(Graph& graph) const {
  const auto depth = graph.get_graph_depth();
  static float constexpr prob = (float)1 / 3;
  for (Graph::Depth current_depth = 1; current_depth <= depth - 2;
       current_depth++) {
    const auto& next_depth_vertex_ids =
        graph.vertexes_of_depth(current_depth + 2);
    for (const auto from_vertex_id : graph.vertexes_of_depth(current_depth)) {
      const auto to_vertex_id = get_random_vertex(next_depth_vertex_ids);
      if (check_probability(prob)) {
        graph.add_edge(from_vertex_id, to_vertex_id);
      }
    }
  }
}

namespace printing {
std::string print_edge_color(const Graph::Edge::Color& color) {
  switch (color) {
    case Graph::Edge::Color::Grey:
      return "grey";
    case Graph::Edge::Color::Yellow:
      return "yellow";
    case Graph::Edge::Color::Red:
      return "red";
    case Graph::Edge::Color::Green:
      return "green";
    default:
      throw std::runtime_error("Failed to determine color");
  }
}
namespace json {
std::string print_graph(const Graph& graph);
std::string print_vertex(const Graph::Vertex&, const Graph& graph);
std::string print_edge(const Graph::Edge& edge);

}  // namespace json
}  // namespace printing

std::string printing::json::print_vertex(const Graph::Vertex& vertex,
                                         const Graph& graph) {
  const auto vertex_id = vertex.id();
  std::string vertex_json =
      "{\"id\":" + std::to_string(vertex_id) + ",\"edge_ids\":[";
  const auto& connected_edge_ids = graph.connected_edge_ids(vertex_id);

  for (const auto& edge_id : connected_edge_ids) {
    vertex_json += std::to_string(edge_id) + ",";
  }

  if (!connected_edge_ids.empty()) {
    vertex_json.pop_back();
  }

  vertex_json +=
      "],\"depth\":" + std::to_string(graph.get_vertex_depth(vertex_id)) + "}";

  return vertex_json;
}

std::string printing::json::print_edge(const Graph::Edge& edge) {
  return "{\"id\":" + std::to_string(edge.id()) + ",\"vertex_ids\":[" +
         std::to_string(edge.from_vertex_id()) + "," +
         std::to_string(edge.to_vertex_id()) + "],\"color\":\"" +
         printing::print_edge_color(edge.color()) + "\"}";
}

std::string printing::json::print_graph(const Graph& graph) {
  std::string graph_json =
      "{\"depth\":" + std::to_string(graph.get_graph_depth()) +
      ",\"vertices\":[";
  const auto& vertices = graph.get_vertexes();

  if (!vertices.empty()) {
    for (const auto& [vertex_id, vertex] : vertices) {
      graph_json += print_vertex(vertex, graph) + ",";
    }

    graph_json.pop_back();
  }

  graph_json += "],\"edges\":[";

  const auto& edges = graph.get_edges();

  if (!edges.empty()) {
    for (const auto& [edge_id, edge] : edges) {
      graph_json += print_edge(edge) + ",";
    }

    graph_json.pop_back();
  }

  graph_json += "]}";

  return graph_json;
}

void write_to_file(const std::string& str, const std::string& filename) {
  std::ofstream out(filename);
  out << str << std::endl;
}

bool is_number(const std::string& str) {
  for (int i = 0; i < str.length(); i++) {
    if (isdigit(str[i]) == false) {
      return false;
    }
  }

  return true;
}

int handle_depth_input() {
  std::string input;
  do {
    std::cout << "Input depth of graph: ";
    std::cin >> input;
    if (is_number(input) && std::stoi(input) >= 0) {
      return std::stoi(input);
    }

    std::cout << "Incorrect depth" << std::endl;
  } while (true);
}

int handle_new_vertexes_count_input() {
  std::string input;
  do {
    std::cout << "Input new_vertexes_count: ";
    std::cin >> input;
    if (is_number(input) && std::stoi(input) >= 0) {
      return std::stoi(input);
    }

    std::cout << "Incorrect new_vertexes_count" << std::endl;
  } while (true);
}

int main() {
  const int depth = handle_depth_input();
  const int new_vertexes_count = handle_new_vertexes_count_input();

  auto params = GraphGenerator::Params(depth, new_vertexes_count);
  const auto generator = GraphGenerator(std::move(params));
  const auto graph = generator.generate();

  const auto graph_json = printing::json::print_graph(graph);
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");

  return 0;
}
