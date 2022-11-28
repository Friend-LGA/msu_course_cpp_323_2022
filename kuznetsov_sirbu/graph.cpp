#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  using Depth = int;

  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }
    Depth depth() const { return depth_; }
    void set_depth(Depth depth) { depth_ = depth; }

   private:
    Depth depth_ = 1;
    VertexId id_ = 0;
  };

  struct Edge {
    enum class Color { Grey, Green, Yellow, Red };

   public:
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
    Color color_ = Color::Grey;
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
  };

  const std::vector<Edge>& get_edges() const { return edges_; }

  const std::vector<Vertex>& get_vertices() const { return vertices_; }

  void add_vertex() {
    VertexId new_vertex_id = get_new_vertex_id_();
    vertices_.emplace_back(new_vertex_id);
    adjacency_list_[new_vertex_id] = {};
    adjacency_list_of_edges_[new_vertex_id] = {};
  }

  void set_vertex_depth(VertexId vertex_id, Depth depth) {
    vertices_.at(vertex_id).set_depth(depth);
    graph_depth_ = std::max(graph_depth_, depth);
    depth_to_vertices_[depth].emplace_back(vertex_id);
  }

  bool has_vertex(VertexId vertex_id) const {
    return adjacency_list_.count(vertex_id);
  }

  bool is_connected(VertexId from_vertex_id, VertexId to_vertex_id) const {
    return std::count(adjacency_list_.at(from_vertex_id).begin(),
                      adjacency_list_.at(from_vertex_id).end(), to_vertex_id);
  }

  const std::vector<EdgeId>& connected_edges_ids(VertexId vertex_id) const {
    if (adjacency_list_of_edges_.count(vertex_id) == 0) {
      const std::vector<EdgeId> empty_edges_list;
      return std::move(empty_edges_list);
    }
    return adjacency_list_of_edges_.at(vertex_id);
  }

  Edge::Color get_edge_color(VertexId from_vertex_id,
                             VertexId to_vertex_id) const {
    const auto from_vertex_depth =
        vertices_.at(from_vertex_id)
            .depth();  // runtime error if edge not in graph
    const auto to_vertex_depth =
        vertices_.at(to_vertex_id)
            .depth();  // runtime error if edge not in graph

    if (from_vertex_id == to_vertex_id) {
      return Edge::Color::Green;
    }
    if (adjacency_list_.at(to_vertex_id).size() == 0) {
      return Edge::Color::Grey;
    }
    if (to_vertex_depth - from_vertex_depth == 1 &&
        !is_connected(from_vertex_id, to_vertex_id)) {
      return Edge::Color::Yellow;
    }
    if (to_vertex_depth - from_vertex_depth == 2) {
      return Edge::Color::Red;
    }
    throw std::runtime_error("Failed to determine color");
  }

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    const auto color = get_edge_color(
        from_vertex_id, to_vertex_id);  // runtime error if edge not in graph

    EdgeId new_id = get_new_edge_id_();
    if (color == Edge::Color::Grey) {
      set_vertex_depth(to_vertex_id, vertices_[from_vertex_id].depth() + 1);
      adjacency_list_[from_vertex_id].emplace_back(to_vertex_id);
      adjacency_list_[to_vertex_id].emplace_back(from_vertex_id);
    }
    adjacency_list_of_edges_[from_vertex_id].emplace_back(new_id);
    adjacency_list_of_edges_[to_vertex_id].emplace_back(new_id);
    edges_.emplace_back(new_id, from_vertex_id, to_vertex_id, color);
  }

  std::vector<Vertex> get_vertices_with_depth(Depth depth) const {
    if (depth_to_vertices_.find(depth) != depth_to_vertices_.end()) {
      return depth_to_vertices_.at(depth);
    } else {
      return {};
    }
  }

  Depth depth() const { return graph_depth_; }

 private:
  VertexId get_new_vertex_id_() { return vertex_id_counter_++; }
  EdgeId get_new_edge_id_() { return edge_id_counter_++; }

  Depth graph_depth_ = 0;
  VertexId vertex_id_counter_ = 0;
  EdgeId edge_id_counter_ = 0;
  std::unordered_map<Depth, std::vector<Vertex> > depth_to_vertices_;
  std::unordered_map<VertexId, std::vector<EdgeId> > adjacency_list_of_edges_;
  std::unordered_map<VertexId, std::vector<VertexId> > adjacency_list_;
  std::vector<Vertex> vertices_;
  std::vector<Edge> edges_;
};

// graph generator

class GraphGenerator {
 public:
  static constexpr double kProbability = 0.33;
  struct Params {
   public:
    Params(Graph::Depth depth, int new_vertices_count)
        : depth_(depth), new_vertices_count_(new_vertices_count) {}

    Graph::Depth depth() const { return depth_; }
    int new_vertices_count() const { return new_vertices_count_; }

   private:
    Graph::Depth depth_ = 0;
    int new_vertices_count_ = 0;
  };

  explicit GraphGenerator(Params&& params) : params_(std::move(params)) {}
  Graph generate() const {
    auto graph = Graph();
    if (params_.depth() < 0) {
      throw std::runtime_error("Graph depth < 0");
    }
    if (params_.depth() > 0) {
      graph.add_vertex();
      generate_grey_edges(graph);
      generate_green_edges(graph);
      generate_yellow_edges(graph);
      generate_red_edges(graph);
    }
    return graph;
  }

 private:
  double generate_random(double a, double b) const {
    std::random_device
        rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(
        rd());  // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(a, b);
    return dis(gen);
  }

  double probaility_generate_grey_edge(Graph::Depth current_depth,
                                       Graph::Depth graph_depth) const {
    if (graph_depth == 0) {
      return 0;
    } else {
      return 1.0 - static_cast<double>((current_depth - 1)) / (graph_depth - 1);
    }
  }

  bool try_generate_grey_edge(Graph& graph,
                              Graph::Depth graph_depth,
                              Graph::Depth current_depth,
                              Graph::VertexId vertex_id,
                              Graph::VertexId& next_vertex_id)
      const {  // generate grey edge: (vertex_id, next_vertex_id)
    if (probaility_generate_grey_edge(current_depth, graph_depth) >
        generate_random(0, 1)) {
      graph.add_vertex();  // add vertex with id = vertex_id
      graph.add_edge(vertex_id, next_vertex_id);
      next_vertex_id++;
      return 1;
    } else {
      return 0;
    }
  }

  void generate_grey_edges(Graph& graph) const {
    std::vector<Graph::VertexId> vertices_with_last_depth = {0};  // root = 0
    Graph::VertexId next_vertex_id = 1;
    for (Graph::Depth current_depth = 1; current_depth <= params_.depth();
         current_depth++) {
      std::vector<Graph::VertexId> new_vertices_with_current_depth;
      for (auto& vertex_id : vertices_with_last_depth) {
        for (int i = 0; i < params_.new_vertices_count(); ++i) {
          if (try_generate_grey_edge(graph, params_.depth(), current_depth,
                                     vertex_id, next_vertex_id)) {
            new_vertices_with_current_depth.emplace_back(next_vertex_id - 1);
          }
        }
      }
      vertices_with_last_depth = new_vertices_with_current_depth;
    }
  }

  void try_generate_yellow_edge(Graph& graph,
                                Graph::Depth graph_depth,
                                const Graph::Vertex& vertex_from,
                                Graph::Vertex& vertex_to) const {
    Graph::Depth vertex_from_depth = vertex_from.depth();
    double probability_generate =
        static_cast<double>((vertex_from_depth - 1)) / (graph_depth - 1);
    if (generate_random(0, 1) > probability_generate) {
      if (graph.is_connected(vertex_from.id(), vertex_to.id())) {
        return;
      }
      graph.add_edge(vertex_from.id(), vertex_to.id());
    }
  }

  void generate_yellow_edges(Graph& graph) const {
    for (auto& vertex_from : graph.get_vertices()) {
      Graph::Depth vertex_depth = vertex_from.depth();
      for (auto& vertex_to :
           graph.get_vertices_with_depth(vertex_from.depth() + 1)) {
        try_generate_yellow_edge(graph, params_.depth(), vertex_from,
                                 vertex_to);
      }
    }
  }

  void try_generate_red_edge(Graph& graph,
                             const Graph::Vertex& vertex_from,
                             Graph::Vertex& vertex_to) const {
    Graph::Depth vertex_from_depth = vertex_from.depth();
    if (generate_random(0, 1) < kProbability) {
      graph.add_edge(vertex_from.id(), vertex_to.id());
    }
  }

  void generate_red_edges(Graph& graph) const {
    for (auto& vertex_from : graph.get_vertices()) {
      Graph::Depth vertex_depth = vertex_from.depth();
      for (auto& vertex_to :
           graph.get_vertices_with_depth(vertex_from.depth() + 2)) {
        try_generate_red_edge(graph, vertex_from, vertex_to);
      }
    }
  }

  void try_generate_green_edge(Graph& graph,
                               const Graph::Vertex& vertex) const {
    if (generate_random(0, 1) < 0.1) {
      graph.add_edge(vertex.id(), vertex.id());
    }
  }

  void generate_green_edges(Graph& graph) const {
    for (auto& vertex : graph.get_vertices()) {
      try_generate_green_edge(graph, vertex);
    }
  }

  Params params_ = Params(0, 0);
};

namespace printing {
namespace json {

std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::string result_json_vertex = "{\"id\":";
  result_json_vertex += std::to_string(vertex.id()) + ",";
  const std::vector<Graph::EdgeId>& edge_ids =
      graph.connected_edges_ids(vertex.id());

  result_json_vertex += "\"edge_ids\":[";
  for (const auto& id : edge_ids) {
    result_json_vertex += std::to_string(id) + ",";
  }

  if (edge_ids.size())
    result_json_vertex.pop_back();
  result_json_vertex += "],";
  result_json_vertex += "\"depth\":" + std::to_string(vertex.depth()) + "}";
  return result_json_vertex;
}

std::string print_edge(const Graph::Edge& edge) {
  std::string result_json_edge =
      "{\"id\":" + std::to_string(edge.id()) + ",\"vertex_ids\":" + "[" +
      std::to_string(edge.from_vertex_id()) + "," +
      std::to_string(edge.to_vertex_id()) + "]," + "\"color\":";
  std::string edge_color;
  switch (edge.color()) {
    case Graph::Edge::Color::Grey:
      edge_color = "grey";
      break;
    case Graph::Edge::Color::Red:
      edge_color = "red";
      break;
    case Graph::Edge::Color::Yellow:
      edge_color = "yellow";
      break;
    case Graph::Edge::Color::Green:
      edge_color = "green";
      break;
  }
  result_json_edge += "\"" + edge_color + "\"" + "}";
  return result_json_edge;
}

std::string print_graph(const Graph& graph) {
  std::string result = "";
  result += "{\"depth\":" + std::to_string(graph.depth()) + ",";
  result += "\"vertices\":[";

  for (const auto& vertex : graph.get_vertices()) {
    result += print_vertex(vertex, graph) + ",";
  }
  if (result.back() == ',')
    result.pop_back();
  result += "],\"edges\":[";

  for (const auto& edge : graph.get_edges()) {
    result += print_edge(edge) + ",";
  }
  if (result.back() == ',')
    result.pop_back();
  result += "]}\n";
  return result;
}
}  // namespace json
}  // namespace printing

void write_to_file(const std::string& graph_json,
                   const std::string& file_name) {
  std::ofstream file(file_name);
  file << graph_json;
}

Graph::Depth handle_depth_input() {
  Graph::Depth depth;
  std::cin >> depth;
  if (depth < 0) {
    throw std::runtime_error("Error: Input must be > 0!");
  }

  return depth;
}

int handle_new_vertices_count_input() {
  int count_vertices;
  std::cin >> count_vertices;
  if (count_vertices < 0) {
    throw std::runtime_error("Error: Input must be > 0!");
  }

  return count_vertices;
}

int main() {
  const int depth = handle_depth_input();
  const int new_vertices_count = handle_new_vertices_count_input();

  auto params = GraphGenerator::Params(depth, new_vertices_count);
  const auto generator = GraphGenerator(std::move(params));
  const auto graph = generator.generate();

  const auto graph_json = printing::json::print_graph(graph);
  write_to_file(graph_json, "graph.json");

  return 0;
}
