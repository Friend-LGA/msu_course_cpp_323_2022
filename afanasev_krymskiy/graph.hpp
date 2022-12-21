#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "interfaces/i_edge.hpp"
#include "interfaces/i_graph.hpp"
#include "interfaces/i_vertex.hpp"

namespace uni_course_cpp {
class Graph : public IGraph {
 public:
  struct Vertex : public IVertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const override { return id_; }

   private:
    VertexId id_ = 0;
  };

  struct Edge : public IEdge {
   public:
    Edge(EdgeId id,
         VertexId from_vertex_id,
         VertexId to_vertex_id,
         EdgeColor color)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id),
          color_(color) {}

    EdgeId id() const override { return id_; }
    VertexId from_vertex_id() const override { return from_vertex_id_; }
    VertexId to_vertex_id() const override { return to_vertex_id_; }
    EdgeColor color() const override { return color_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
    EdgeColor color_ = EdgeColor::Grey;
  };

  VertexId add_vertex() override;

  EdgeId add_edge(VertexId, VertexId) override;

  const std::vector<EdgeId>& connected_edge_ids(VertexId id) const override {
    return adjacency_list_.at(id);
  }

  const std::unordered_map<VertexId, std::unique_ptr<IVertex>>& vertices()
      const override {
    return vertices_;
  }

  const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& edges()
      const override {
    return edges_;
  }

  GraphDepth depth() const override;

  GraphDepth vertex_depth(VertexId vertex_id) const override {
    return depths_.at(vertex_id);
  }

  const std::vector<VertexId>& vertices_at_depth(
      GraphDepth depth) const override {
    return vertices_at_depth_.at(depth);
  }

  bool has_edge(VertexId, VertexId) const override;

  const std::vector<EdgeId>& color_edge_ids(EdgeColor color) const override;

 private:
  VertexId current_vertex_id_ = 0;
  EdgeId current_edge_id_ = 0;

  VertexId next_vertex_id() { return current_vertex_id_++; }

  EdgeId next_edge_id() { return current_edge_id_++; }

  bool has_vertex(VertexId vertex_id) const {
    return vertices_.find(vertex_id) != vertices_.end();
  }

  void set_vertex_depth(VertexId, GraphDepth);

  EdgeColor determine_color(VertexId, VertexId) const;

  std::unordered_map<VertexId, std::unique_ptr<IVertex>> vertices_;
  std::unordered_map<EdgeId, std::unique_ptr<IEdge>> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<VertexId, GraphDepth> depths_;
  std::vector<std::vector<VertexId>> vertices_at_depth_;
  std::unordered_map<EdgeColor, std::vector<EdgeId>> color_edge_ids_;
};

constexpr GraphDepth kYellowEdgeDepth = 1;
constexpr GraphDepth kRedEdgeDepth = 2;
constexpr GraphDepth kDefaultDepth = 1;
}  // namespace uni_course_cpp
