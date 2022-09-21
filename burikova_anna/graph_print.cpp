#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int kVerticesCount = 14;

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
  std::vector<Vertex> get_vertices() const { return vertices_; }
  std::vector<Edge> const get_edges() const { return edges_; }
  void add_vertex();
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

 private:
  std::vector<Vertex> vertices_;
  std::vector<Edge> edges_;
  int last_vertex_id_ = 0;
  int last_edge_id_ = 0;
  VertexId get_new_vertex_id() { return last_vertex_id_++; }
  EdgeId get_new_edge_id() { return last_edge_id_++; }
};

void Graph::add_vertex() {
  vertices_.emplace_back(get_new_vertex_id());
}

void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  edges_.emplace_back(get_new_edge_id(), from_vertex_id, to_vertex_id);
}

namespace printing {
namespace json {
std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::vector<Graph::Edge> edges = graph.get_edges();
  std::string string_vertex;
  string_vertex =
      "{\n\"id\": " + std::to_string(vertex.id()) + ",\n\"edges\": ";
  string_vertex += "[";
  for (const auto& edge : edges) {
    if (edge.to_vertex_id() == vertex.id()) {
      string_vertex += std::to_string(edge.id());
      string_vertex += ", ";
    }
  }
  for (const auto& edge : edges) {
    if (edge.from_vertex_id() == vertex.id()) {
      string_vertex += std::to_string(edge.id());
      string_vertex += ", ";
    }
  }
  string_vertex.pop_back();
  string_vertex.pop_back();
  string_vertex += "]";
  string_vertex += "\n}";
  return string_vertex;
}
std::string print_edge(const Graph::Edge& edge, const Graph& graph) {
  std::string string_edge;
  string_edge =
      "{\n\"id\": " + std::to_string(edge.id()) + ",\n\"vertex_ids\": ";
  string_edge = string_edge + "[" + std::to_string(edge.from_vertex_id()) +
                ", " + std::to_string(edge.to_vertex_id()) + "]";
  string_edge += "\n}";
  return string_edge;
}
std::string print_graph(const Graph& graph) {
  std::string string_graph;
  std::vector<Graph::Vertex> vertices = graph.get_vertices();
  std::vector<Graph::Edge> edges = graph.get_edges();
  string_graph = "{\n\"vertices\": [\n";
  for (const auto& vertex : vertices) {
    string_graph += print_vertex(vertex, graph);
    string_graph += "\n";
  }
  string_graph += "],\n";
  string_graph += "\"edges\": [\n";
  for (const auto& edge : edges) {
    string_graph += print_edge(edge, graph);
    string_graph += "\n";
  }
  string_graph += "]\n}";
  return string_graph;
}
}  // namespace json
}  // namespace printing

Graph generate_graph() {
  auto graph = Graph();

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
  return graph;
}

void write_to_file(std::string input_string, std::string file_name) {
  std::ofstream fout(file_name);
  fout << input_string;
  fout.close();
}

int main() {
  const auto graph = generate_graph();
  const auto graph_json = printing::json::print_graph(graph);
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");

  return 0;
}
