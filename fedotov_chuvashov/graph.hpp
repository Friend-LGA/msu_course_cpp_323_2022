#pragma once
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>
#include "interfaces/i_edge.hpp"
#include "interfaces/i_graph.hpp"
#include "interfaces/i_vertex.hpp"

namespace uni_course_cpp {
class Graph : public IGraph {
 public:
  static constexpr GraphDepth kGraphBaseDepth = 1;

  VertexId add_vertex() override;

  EdgeId add_edge(VertexId first_vertex_id, VertexId second_vertex_id) override;

  const std::unordered_map<VertexId, std::unique_ptr<IVertex>>& vertices()
      const override {
    return vertices_;
  };

  const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& edges()
      const override {
    return edges_;
  };

  const std::vector<EdgeId>& connected_edge_ids(
      VertexId vertex_id) const override {
    return adjacency_list_.at(vertex_id);
  }

  bool is_connected(VertexId from_vertex_id,
                    VertexId to_vertex_id) const override;

  EdgeColor get_edge_color(VertexId from_vertex_id,
                           VertexId to_vertex_id) const override;

  GraphDepth get_vertex_depth(VertexId vertex_id) const override {
    return depths_of_vertices_.at(vertex_id);
  }

  void set_vertex_depth(VertexId vertex_id, GraphDepth new_depth) override;

  const std::vector<EdgeId>& get_colored_edge_ids(
      EdgeColor color) const override;

  GraphDepth depth() const override { return vertices_at_depth_.size(); }

  const std::set<VertexId>& vertices_at_depth(GraphDepth depth) const override {
    return vertices_at_depth_.at(depth - kGraphBaseDepth);
  }

  std::set<VertexId> children_of_vertex(VertexId vertex_id) const override;

 private:
  struct Vertex : public IVertex {
   public:
    explicit Vertex(VertexId id) : id_(id){};
    VertexId id() const override { return id_; };

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

    EdgeId id() const override { return id_; };

    VertexId from_vertex_id() const override { return from_vertex_id_; };

    VertexId to_vertex_id() const override { return to_vertex_id_; };

    EdgeColor color() const override { return color_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
    EdgeColor color_ = EdgeColor::Grey;
  };

  std::set<VertexId>& get_vertex_ids_at_depth(GraphDepth depth) {
    return vertices_at_depth_[depth - kGraphBaseDepth];
  }

  bool has_vertex(VertexId id) const {
    return vertices_.find(id) != vertices_.end();
  };

  VertexId get_new_vertex_id() { return last_vertex_id_++; };

  EdgeId get_new_edge_id() { return last_edge_id_++; };

  std::unordered_map<EdgeColor, std::vector<EdgeId>> colored_edge_ids_;
  std::unordered_map<VertexId, GraphDepth> depths_of_vertices_;
  std::vector<std::set<VertexId>> vertices_at_depth_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<VertexId, std::unique_ptr<IVertex>> vertices_;
  std::unordered_map<EdgeId, std::unique_ptr<IEdge>> edges_;
  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
};

constexpr GraphDepth kYellowDepthDifference = 1;
constexpr GraphDepth kRedDepthDifference = 2;
constexpr GraphDepth kGraphBaseDepth = 1;
constexpr GraphDepth kGreyDepthDifference = 1;

}  // namespace uni_course_cpp
