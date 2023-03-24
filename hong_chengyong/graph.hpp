#pragma once
#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
namespace uni_course_cpp {
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
}  // namespace uni_course_cpp