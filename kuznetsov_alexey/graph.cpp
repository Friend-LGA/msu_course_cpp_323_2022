#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

class Graph {
 public:
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
    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id) {}
    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; }
    VertexId to_vertex_id() const { return to_vertex_id_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
  };

  void add_vertex() {
    const VertexId new_vertex_id = get_new_vertex_id_();
    vertices_.emplace_back(new_vertex_id);
    adjacency_list_[new_vertex_id] = {};
  }

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    assert(has_vertex(from_vertex_id));
    assert(has_vertex(to_vertex_id));
    const EdgeId edge_id = get_new_edge_id_();
    edges_.emplace_back(edge_id, from_vertex_id, to_vertex_id);
    if (from_vertex_id != edge_id) {
      adjacency_list_[from_vertex_id].emplace_back(edge_id);
    }
    adjacency_list_[to_vertex_id].emplace_back(edge_id);
  }

  bool has_vertex(VertexId vertex_id) const {
    return adjacency_list_.find(vertex_id) != adjacency_list_.end();
  }

  const std::vector<Vertex>& get_vertices() const { return vertices_; }

  const std::vector<Edge>& get_edges() const { return edges_; }

  const std::vector<EdgeId>& connected_edges_ids(VertexId vertex_id) const {
    if (!has_vertex(vertex_id)) {
      static std::vector<EdgeId> empty_edges_list;
      return empty_edges_list;
    }
    return adjacency_list_.at(vertex_id);
  }

 private:
  VertexId get_new_vertex_id_() { return vertex_id_counter_++; }

  EdgeId get_new_edge_id_() { return edge_id_counter_++; }

  VertexId vertex_id_counter_ = 0;
  EdgeId edge_id_counter_ = 0;
  std::vector<Vertex> vertices_;
  std::vector<Edge> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
};

namespace printing {
namespace json {

std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::string result = "{\"id\":";
  result += std::to_string(vertex.id()) + ",";

  const std::vector<Graph::EdgeId>& edge_ids =
      graph.connected_edges_ids(vertex.id());

  result += "\"edge_ids\":[";
  for (const auto& id : edge_ids) {
    result += std::to_string(id) + ",";
  }
  if (edge_ids.size())
    result.pop_back();
  result += "]}";
  return result;
}

std::string print_edge(const Graph::Edge& edge) {
  return "{\"id\":" + std::to_string(edge.id()) + ",\"vertex_ids\":" + "[" +
         std::to_string(edge.from_vertex_id()) + "," +
         std::to_string(edge.to_vertex_id()) + "]}";
}

std::string print_graph(const Graph& graph) {
  std::string result = "{\"vertices\":[";
  for (const auto& vertex : graph.get_vertices()) {
    result += print_vertex(vertex, graph) + ",";
  }
  if (result.back() == ',') {
    result.pop_back();
  }
  result += "],\"edges\":[";
  for (const auto& edge : graph.get_edges()) {
    result += print_edge(edge) + ",";
  }
  if (result.back() == ',') {
    result.pop_back();
  }
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

int main() {
  auto graph = Graph();

  constexpr int kVerticesCount = 14;

  for (int i = 0; i < kVerticesCount; i++) {
    graph.add_vertex();
  }

  graph.add_edge(0, 1);
  graph.add_edge(0, 2);
  graph.add_edge(0, 3);
  graph.add_edge(1, 4);
  graph.add_edge(1, 5);
  graph.add_edge(1, 6);
  graph.add_edge(2, 7);
  graph.add_edge(2, 8);
  graph.add_edge(3, 9);
  graph.add_edge(4, 10);
  graph.add_edge(5, 10);
  graph.add_edge(6, 10);
  graph.add_edge(7, 11);
  graph.add_edge(8, 11);
  graph.add_edge(9, 12);
  graph.add_edge(10, 13);
  graph.add_edge(11, 13);
  graph.add_edge(12, 13);

  const auto graph_json = printing::json::print_graph(graph);
  write_to_file(graph_json, "graph.json");
  return 0;
}