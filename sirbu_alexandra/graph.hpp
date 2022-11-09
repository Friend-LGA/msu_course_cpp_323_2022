#include <string>
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

  VertexId add_vertex() {
    const VertexId new_vertex_id = generate_vertex_id();
    vertices_.emplace(new_vertex_id, new_vertex_id);
    adjacency_list_[new_vertex_id];

    return new_vertex_id;
  }

  EdgeId add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    const EdgeId new_edge_id = generate_edge_id();

    edges_.emplace(new_edge_id, Edge(new_edge_id, new_edge_id, to_vertex_id));

    adjacency_list_[from_vertex_id].push_back(new_edge_id);
    adjacency_list_[to_vertex_id].push_back(new_edge_id);

    if (from_vertex_id != to_vertex_id) {
      adjacency_list_[to_vertex_id].push_back(new_edge_id);
    }

    return new_edge_id;
  }

  const std::vector<EdgeId>& connected_edge_ids(VertexId id) const {
    return adjacency_list_.at(id);
  }

  const std::unordered_map<VertexId, Vertex>& vertices() const {
    return vertices_;
  }

  const std::unordered_map<EdgeId, Edge>& edges() const { return edges_; }

 private:
  EdgeId next_edge_id_ = 0;
  VertexId next_vertex_id_ = 0;
  EdgeId generate_edge_id() { return next_edge_id_++; }
  VertexId generate_vertex_id() { return next_vertex_id_++; }

  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
};

namespace printing {
namespace json {
std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::string result = "{";

  result += "\"id\":";
  result += std::to_string(vertex.id());

  result += ",";

  result += "\"edge_ids\":[";
  const auto& edge_ids = graph.connected_edge_ids(vertex.id());
  for (const auto edge_id : edge_ids) {
    result += std::to_string(edge_id);
    result += ",";
  }
  if (!edge_ids.empty()) {
    result.pop_back();
  }

  result += "]";

  result += "}";
  return result;
}
std::string print_edge(const Graph::Edge& edge) {
  std::string result = "{";

  result += "\"id\":";
  result += std::to_string(edge.id());

  result += ",";

  result += "\"vertex_ids\":[";
  result += std::to_string(edge.from_vertex_id());
  result += ",";
  result += std::to_string(edge.to_vertex_id());
  result += "]";

  result += "}";
  return result;
}
std::string print_graph(const Graph& graph) {
  std::string result = "{";

  result += "\"vertices\":[";
  const auto& vertices = graph.vertices();
  for (const auto& [vertex_id, vertex] : vertices) {
    result += print_vertex(vertex, graph);
    result += ",";
  }
  if (!vertices.empty()) {
    result.pop_back();
  }

  result += "]";

  result += ",";

  result += "\"edges\":[";
  const auto& edges = graph.edges();
  for (const auto& [edge_id, edge] : edges) {
    result += print_edge(edge);
    result += ",";
  }
  if (!edges.empty()) {
    result.pop_back();
  }

  result += "]";

  result += "}\n";
  return result;
}
}  // namespace json
};  // namespace printing
