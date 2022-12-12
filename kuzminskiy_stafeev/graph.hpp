#pragma once
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {

class Graph : public IGraph {
 public:
  bool is_connected(VertexId, VertexId) const override;
  VertexId add_vertex() override;

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) override;

  GraphDepth get_graph_depth() const override {
    return vertices_of_depth_.size() ? vertices_of_depth_.size() - 1 : 0;
  }
  GraphDepth get_vertex_depth(VertexId id) const override;

  const std::unordered_map<VertexId, std::unique_ptr<IVertex>>& vertices()
      const override {
    return vertices_;
  }

  const std::vector<VertexId>& vertices_of_depth(
      GraphDepth depth) const override {
    return vertices_of_depth_[depth];
  };

  const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& edges()
      const override {
    return edges_;
  }
  const std::set<EdgeId>& connected_edge_ids(
      VertexId vertex_id) const override {
    return connections_list_.at(vertex_id);
  }

  const std::vector<EdgeId>& edges_ids_of_color(EdgeColor color) const override;

  bool has_vertex(VertexId id) const override {
    return vertices_.find(id) != vertices_.end();
  }
  bool has_edge(EdgeId id) const override {
    return edges_.find(id) != edges_.end();
  }

 private:
  struct Vertex : public IVertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }

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

    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; }
    VertexId to_vertex_id() const { return to_vertex_id_; }
    EdgeColor color() const { return color_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
    EdgeColor color_ = EdgeColor::Grey;
  };

  void add_edge_in_color(EdgeId edge_id, const EdgeColor& color);
  EdgeId gen_new_edge_id() { return next_edge_id_++; }
  VertexId gen_new_vertex_id() { return next_vertex_id_++; }

  EdgeColor define_color(VertexId from_vertex_id, VertexId to_vertex_id) const;
  void set_vertex_depth(VertexId id, GraphDepth depth);

  VertexId next_vertex_id_ = 0;
  EdgeId next_edge_id_ = 0;
  std::unordered_map<VertexId, GraphDepth> depth_of_vertices_;
  std::vector<std::vector<VertexId>> vertices_of_depth_;
  std::unordered_map<VertexId, std::unique_ptr<IVertex>> vertices_;
  std::unordered_map<EdgeId, std::unique_ptr<IEdge>> edges_;
  std::unordered_map<VertexId, std::set<EdgeId>> connections_list_;
  std::unordered_map<EdgeColor, std::vector<EdgeId>> edges_ids_of_color_;
};

}  // namespace uni_course_cpp
