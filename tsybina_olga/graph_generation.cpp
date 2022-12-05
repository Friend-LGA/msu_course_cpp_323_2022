#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  void add_vertex() {
    const VertexId new_vertex_id = get_new_vertex_id();
    vertices_.emplace_back(Vertex(new_vertex_id));
    connections_[new_vertex_id] = {};
  };

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    const auto edge_id = get_new_edge_id();
    edges_.emplace_back(Edge(edge_id, from_vertex_id, to_vertex_id));
    connections_[from_vertex_id].push_back(edge_id);
    if (to_vertex_id != from_vertex_id) {
      connections_[to_vertex_id].push_back(edge_id);
    }
  };

  const auto& vertices() const { return vertices_; }

  const auto& edges() const { return edges_; }

  const auto& connections() const { return connections_; }

  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; };

   private:
    VertexId id_ = 0;
  };
  struct Edge {
   public:
    explicit Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id) {}
    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; };
    VertexId to_vertex_id() const { return to_vertex_id_; };

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
  };

 private:
  std::vector<Vertex> vertices_;
  std::vector<Edge> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> connections_;

  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;

  VertexId get_new_vertex_id() { return last_vertex_id_++; }
  EdgeId get_new_edge_id() { return last_edge_id_++; }
};

namespace printing {
namespace json {

// return json like {"id":0,"edge_ids":[0,1,2]}
std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::ostringstream stream;
  stream << "{";
  stream << R"("id":)" << vertex.id() << ",";
  stream << R"("edge_ids":[)";
  std::string separator = "";

  for (auto edge_id : graph.connections().at(vertex.id())) {
    stream << separator << edge_id;
    separator = ",";
  }

  stream << "]";
  stream << "}";

  return stream.str();
}

// return json like {"id":0,"vertex_ids":[0,1]}
std::string print_edge(const Graph::Edge& edge) {
  std::ostringstream stream;
  stream << "{";
  stream << R"("id":)" << edge.id() << ",";
  stream << R"("vertex_ids":[)" << edge.from_vertex_id() << ","
         << edge.to_vertex_id() << "]";
  stream << "}";
  return stream.str();
}

std::string print_graph(const Graph& graph) {
  std::ostringstream stream;
  stream << "{";
  stream << R"("vertices":)"
         << "[";

  std::string separator = "";

  for (const auto& vertex : graph.vertices()) {
    stream << separator << print_vertex(vertex, graph);
    separator = ",";
  }

  stream << "],"
         << R"("edges":)"
         << "[";
  separator = "";

  for (const auto& edge : graph.edges()) {
    stream << separator << print_edge(edge);
    separator = ",";
  }

  stream << "]";
  stream << "}";

  return stream.str();
}

}  // namespace json
}  // namespace printing

Graph generate_graph() {
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

  return graph;
}

void write_to_file(const std::string& data, const std::string& file_path) {
  std::ofstream out(file_path, std::ios::out | std::ios::trunc);
  out << data << "\n";
  out.close();
}

int main() {
  const auto graph = generate_graph();
  const auto graph_json = printing::json::print_graph(graph);
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");

  return 0;
}
