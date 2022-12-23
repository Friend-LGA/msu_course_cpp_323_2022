#include <cassert>
#include <stdexcept>
#include <unordered_map>
#include <vector>
namespace uni_course_cpp {
using VertexId = int;
using EdgeId = int;

struct Edge {
  const EdgeId id;
  const VertexId from_vertex_id;
  const VertexId to_vertex_id;
  Edge(const EdgeId& _id, VertexId from_vertex_id_, VertexId to_vertex_id_)
      : id(_id), from_vertex_id(from_vertex_id_), to_vertex_id(to_vertex_id_) {}
};

struct Vertex {
  const VertexId id;
  explicit Vertex(const VertexId& _id) : id(_id) {}
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

  bool isConnected(const VertexId& from_vertex_id,
                   const VertexId& to_vertex_id) const {
    assert(hasVertex(from_vertex_id) && "from Vertex index is out of range");
    assert(hasVertex(to_vertex_id) && "to Vertex index is out of range");
    if (from_vertex_id == to_vertex_id) {
      for (const auto& edge_id : connection_list_.at(from_vertex_id)) {
        const auto& edge = edges_[edge_id];
        if (edge.from_vertex_id == from_vertex_id &&
            edge.to_vertex_id == from_vertex_id) {
          return true;
        }
      }
    } else {
      for (const auto& edge_id : connection_list_.at(from_vertex_id)) {
        const auto& edge = edges_[edge_id];
        if (edge.from_vertex_id == to_vertex_id ||
            edge.to_vertex_id == to_vertex_id) {
          return true;
        }
      }
    }
    return false;
  }

  void addVertex() {
    const auto& new_vertex = vertexes_.emplace_back(getNewVertexId());
    connection_list_.insert({new_vertex.id, std::vector<EdgeId>()});
  }

  void addEdge(const VertexId& from_vertex_id, const VertexId& to_vertex_id) {
    assert(hasVertex(from_vertex_id) && "Vertex1 index is out of range");
    assert(hasVertex(to_vertex_id) && "Vertex2 index is out of range");
    assert(!isConnected(from_vertex_id, to_vertex_id) &&
           "These vertexes are already connected");
    const auto& new_edge =
        edges_.emplace_back(getNewEdgeId(), from_vertex_id, to_vertex_id);
    connection_list_[from_vertex_id].push_back(new_edge.id);
    if (from_vertex_id != to_vertex_id) {
      connection_list_[to_vertex_id].push_back(new_edge.id);
    }
  }

  const std::vector<EdgeId> vertexConnections(const VertexId& id) const {
    assert(hasVertex(id) && "Vertex id is out of range");
    return connection_list_.at(id);
  }
  const std::vector<Vertex>& vertexes() const { return vertexes_; }
  const std::vector<Edge>& edges() const { return edges_; }

 private:
  std::vector<Vertex> vertexes_;
  std::vector<Edge> edges_;
  VertexId new_vertex_id_ = 0;
  EdgeId new_edge_id_ = 0;
  std::unordered_map<VertexId, std::vector<EdgeId>> connection_list_;
  VertexId getNewVertexId() { return new_vertex_id_++; }
  EdgeId getNewEdgeId() { return new_edge_id_++; }
};
}  // namespace uni_course_cpp
