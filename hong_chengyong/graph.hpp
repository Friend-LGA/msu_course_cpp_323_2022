#pragma once
#include <array>
#include <cassert>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {

using VertexId = int;
using EdgeId = int;

struct Vertex {
  const VertexId id;

  explicit Vertex(const VertexId& _id) : id(_id) {}
};

struct Edge {
  enum class Color { Gray, Green, Yellow, Red };
  static constexpr std::array<Edge::Color, 4> ALL_COLORS = {
      Color::Gray, Color::Green, Color::Yellow, Color::Red};
  const EdgeId id;
  const VertexId from_vertex_id;
  const VertexId to_vertex_id;
  const Color color;
  Edge(const EdgeId& _id,
       const VertexId& _from_vertex_id,
       const VertexId& _to_vertex_id,
       const Color& _color)
      : id(_id),
        from_vertex_id(_from_vertex_id),
        to_vertex_id(_to_vertex_id),
        color(_color) {}
};

class Graph {
 public:
  using Depth = int;
  const int kStart = 0;
  bool hasVertex(const VertexId& vertex_id) const {
    for (const auto& vertex : vertices_) {
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

  bool isConnected(const VertexId& from_vertex_id,
                   const VertexId& to_vertex_id) const {
    assert(hasVertex(from_vertex_id) && "from_vertex index is out of range");
    assert(hasVertex(to_vertex_id) && "to_vertex index is out of range");
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
    const auto& new_vertex = vertices_.emplace_back(getNewVertexId());
    connection_list_.insert({new_vertex.id, std::vector<EdgeId>()});
    layers_list_[kStart].push_back(new_vertex.id);
    vertices_depths_[new_vertex.id] = 0;
    return new_vertex.id;
  }

  void addEdge(const VertexId& from_vertex_id, const VertexId& to_vertex_id) {
    assert(hasVertex(from_vertex_id) && "from_vertex index is out of range");
    assert(hasVertex(to_vertex_id) && "to_vertex index is out of range");
    assert(!isConnected(from_vertex_id, to_vertex_id) &&
           "These vertexes are already connected");
    const auto color = calculateEdgeColor(from_vertex_id, to_vertex_id);
    if (color == Edge::Color::Gray) {
      grayEdgeInitialization(from_vertex_id, to_vertex_id);
    }
    const auto& new_edge = edges_.emplace_back(getNewEdgeId(), from_vertex_id,
                                               to_vertex_id, color);
    color_list_[color].push_back(new_edge.id);
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
  const std::vector<Vertex>& vertices() const { return vertices_; }
  const std::vector<Edge>& edges() const { return edges_; }
  const std::vector<VertexId>& vertexIdsAtLayer(int depth) const {
    assert(depth <= layers_list_.size() && "Graph is not that deep");
    return layers_list_.at(depth);
  }
  int vertexDepth(const VertexId& vertex_id) const {
    assert(hasVertex(vertex_id) && "Vertex id is out of range");
    return vertices_depths_.at(vertex_id);
  }
  Depth depth() const { return layers_list_.size(); }

 private:
  std::vector<Vertex> vertices_;
  std::vector<Edge> edges_;
  VertexId vertex_new_id_ = 0;
  EdgeId edge_new_id_ = 0;
  std::unordered_map<VertexId, std::vector<EdgeId>> connection_list_;
  std::unordered_map<int, std::vector<VertexId>> layers_list_;
  std::unordered_map<VertexId, Depth> vertices_depths_;
  std::unordered_map<Edge::Color, std::vector<EdgeId>> color_list_;
  VertexId getNewVertexId() { return vertex_new_id_++; }
  EdgeId getNewEdgeId() { return edge_new_id_++; }
  Edge::Color calculateEdgeColor(const VertexId& from_vertex_id,
                                 const VertexId& to_vertex_id) const {
    if (connection_list_.at(to_vertex_id).size() == 0 ||
        connection_list_.at(from_vertex_id).size() == 0) {
      return Edge::Color::Gray;
    } else if (from_vertex_id == to_vertex_id) {
      return Edge::Color::Green;
    } else if (std::abs(vertexDepth(to_vertex_id) -
                        vertexDepth(from_vertex_id)) ==
               1)  // if the interval between vertices = 1
    {
      return Edge::Color::Yellow;
    } else  // if (std::abs(vertexDepth(to_vertex_id) -
            //             vertexDepth(from_vertex_id)) ==
            //   2)  // if the interval between vertices = 2
      // {
      return Edge::Color::Red;
    // }
  }
  void grayEdgeInitialization(const VertexId& from_vertex_id,
                              const VertexId& to_vertex_id) {
    int min_vertex_id = std::min(from_vertex_id, to_vertex_id);
    int max_vertex_id = std::max(from_vertex_id, to_vertex_id);
    Graph::Depth new_depth = vertices_depths_[min_vertex_id] + 1;
    vertices_depths_[max_vertex_id] = new_depth;
    layers_list_[new_depth].push_back(max_vertex_id);
    auto front_layer = layers_list_[0];
    for (auto it = front_layer.begin(); it != front_layer.end(); it++) {
      if (*it == max_vertex_id) {
        front_layer.erase(it);
        break;
      }
    }
  }
};
}  // namespace uni_course_cpp
