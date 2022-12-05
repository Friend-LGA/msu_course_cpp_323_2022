#pragma once

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  using Depth = int;

  static constexpr Graph::Depth kGraphBaseDepth = 1;
  static constexpr Graph::Depth kRedEdgeDepthJump = 2;
  static constexpr Graph::Depth kYellowEdgeDepthJump = 1;

  VertexId add_vertex() {
    const VertexId new_vertex_id = get_new_vertex_id();
    vertices_.emplace(new_vertex_id, new_vertex_id);
    connections_[new_vertex_id] = {};
    set_vertex_depth(new_vertex_id, kGraphBaseDepth);
    return new_vertex_id;
  }

  bool has_vertex(VertexId vertex_id) const {
    return vertices_.find(vertex_id) != vertices_.end();
  }

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    assert(has_vertex(from_vertex_id));
    assert(has_vertex(to_vertex_id));

    const auto edge_id = get_new_edge_id();
    const auto edge_color = define_edge_color(from_vertex_id, to_vertex_id);
    edges_.emplace(edge_id,
                   Edge(edge_id, from_vertex_id, to_vertex_id, edge_color));

    connections_[from_vertex_id].push_back(edge_id);
    if (to_vertex_id != from_vertex_id) {
      connections_[to_vertex_id].push_back(edge_id);
    }

    if (edge_color == Edge::Color::Grey) {
      set_vertex_depth(to_vertex_id, depth_of(from_vertex_id) + 1);
    }
  }

  const auto& vertices() const { return vertices_; }

  const auto& edges() const { return edges_; }

  const auto& connections_of(VertexId vertex_id) const {
    return connections_.at(vertex_id);
  }

  Graph::Depth depth() const { return depth_map_.size() - 1; }

  Depth depth_of(VertexId vertex_id) const {
    return vertex_depths_.at(vertex_id);
  }

  const std::unordered_set<VertexId>& vertices_at_depth(Depth depth) const {
    assert(depth < depth_map_.size());
    return depth_map_.at(depth);
  }

  std::vector<VertexId> connected_vertices(VertexId vertex_id) const {
    std::vector<VertexId> result;
    result.reserve(connections_.at(vertex_id).size());
    for (EdgeId edge_id : connections_.at(vertex_id)) {
      result.push_back(other_end_of(edge_id, vertex_id));
    }
    return result;
  }

  bool is_connected(VertexId from_vertex_id, VertexId to_vertex_id) const {
    const std::vector<VertexId>& from_vertex_ids =
        connected_vertices(from_vertex_id);
    return std::find(from_vertex_ids.begin(), from_vertex_ids.end(),
                     to_vertex_id) != from_vertex_ids.end();
  }

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

    explicit Edge(EdgeId id,
                  VertexId from_vertex_id,
                  VertexId to_vertex_id,
                  Color color)
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
    Color color_;
  };

 private:
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> connections_;
  std::unordered_map<VertexId, Depth> vertex_depths_;
  std::vector<std::unordered_set<VertexId>> depth_map_;

  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;

  VertexId get_new_vertex_id() { return last_vertex_id_++; }
  EdgeId get_new_edge_id() { return last_edge_id_++; }

  Edge::Color define_edge_color(VertexId from_vertex_id,
                                VertexId to_vertex_id) const {
    if (from_vertex_id == to_vertex_id) {
      return Edge::Color::Green;
    }
    if (depth_of(to_vertex_id) == kGraphBaseDepth) {
      return Edge::Color::Grey;
    }
    const Depth depth_jump = depth_of(to_vertex_id) - depth_of(from_vertex_id);
    if (depth_jump == kYellowEdgeDepthJump &&
        !is_connected(from_vertex_id, to_vertex_id)) {
      return Edge::Color::Yellow;
    }
    if (depth_jump == kRedEdgeDepthJump) {
      return Edge::Color::Red;
    }
    throw std::runtime_error("Failed to determine color");
  }

  void set_vertex_depth(VertexId vertex_id, Depth depth) {
    const auto depth_iterator = vertex_depths_.find(vertex_id);
    if (depth_iterator != vertex_depths_.end()) {
      Depth old_depth = depth_iterator->second;
      assert(depth != old_depth);
      depth_map_[old_depth].erase(vertex_id);
    }

    vertex_depths_[vertex_id] = depth;
    if (depth_map_.size() <= depth) {
      depth_map_.resize(depth + 1);
    }
    depth_map_[depth].insert(vertex_id);
  }

  VertexId other_end_of(EdgeId edge_id, VertexId vertex_id) const {
    const Edge& edge = edges().at(edge_id);
    assert(edge.from_vertex_id() == vertex_id ||
           edge.to_vertex_id() == vertex_id);
    if (edge.from_vertex_id() == vertex_id) {
      return edge.to_vertex_id();
    }
    return edge.from_vertex_id();
  }
};
