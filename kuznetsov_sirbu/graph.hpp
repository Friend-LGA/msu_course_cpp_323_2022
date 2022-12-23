#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {

class Graph : public IGraph {
 public:
  static constexpr GraphDepth kBaseDepth = 1;
  static constexpr GraphDepth kDifferenceRedEdge = 2;
  static constexpr GraphDepth kDifferenceYellowEdge = 1;

  VertexId add_vertex() override;

  EdgeId add_edge(VertexId from_vertex_id, VertexId to_vertex_id) override;

  bool has_vertex(VertexId vertex_id) const override {
    return adjacency_list_.find(vertex_id) != adjacency_list_.end();
  }

  const std::vector<std::unique_ptr<IVertex>>& get_vertices() const override {
    return vertices_;
  }

  const std::vector<std::unique_ptr<IEdge>>& get_edges() const override {
    return edges_;
  }

  const std::vector<EdgeId>& connected_edges_ids(
      VertexId vertex_id) const override;

  void set_vertex_depth(VertexId vertex_id, GraphDepth depth) override;

  bool is_connected(VertexId from_vertex_id,
                    VertexId to_vertex_id) const override;

  EdgeColor get_edge_color(VertexId from_vertex_id,
                           VertexId to_vertex_id) const override;

  GraphDepth vertex_depth(VertexId vertex_id) const override {
    return vertex_depths_.at(vertex_id);
  }

  const std::vector<VertexId>& get_vertices_with_depth(
      GraphDepth depth) const override;

  const std::vector<EdgeId>& get_edge_ids_with_color(
      EdgeColor color) const override;

  GraphDepth depth() const override { return depth_to_vertices_.size(); }

 private:
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
    EdgeColor color_ = EdgeColor::Grey;
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
  };

  VertexId get_new_vertex_id() { return vertex_id_counter_++; }
  EdgeId get_new_edge_id() { return edge_id_counter_++; }

  VertexId vertex_id_counter_ = 0;
  EdgeId edge_id_counter_ = 0;
  std::vector<std::unique_ptr<IVertex>> vertices_;
  std::vector<std::unique_ptr<IEdge>> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<VertexId, GraphDepth> vertex_depths_;
  std::unordered_map<GraphDepth, std::vector<VertexId>> depth_to_vertices_;
  std::unordered_map<EdgeColor, std::vector<EdgeId>> color_to_edges_;
};

}  // namespace uni_course_cpp
