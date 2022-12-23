#pragma once

#include <unordered_map>
#include <vector>
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {
class Graph : public IGraph {
 public:
  VertexId add_vertex() override;

  EdgeId add_edge(VertexId first_vertex_id, VertexId second_vertex_id) override;

  bool has_edge(VertexId first_vertex_id,
                VertexId second_vertex_id) const override;

  GraphDepth depth() const override { return depth_levels_.size(); }

  const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& get_edges()
      const override {
    return edges_;
  }

  const std::unordered_map<VertexId, std::unique_ptr<IVertex>>& get_vertices()
      const override {
    return vertices_;
  }

  const std::vector<EdgeId>& get_connected_edge_ids(
      VertexId vertex_id) const override {
    return connections_.at(vertex_id);
  }

  const std::vector<std::vector<VertexId>>& get_depth_levels() const override {
    return depth_levels_;
  };

  GraphDepth get_vertex_depth(VertexId vertex_id) const override;

  const std::vector<VertexId>& get_vertex_ids_at_depth(
      GraphDepth depth) const override;

  const std::vector<EdgeId>& get_colored_edge_ids(
      EdgeColor color) const override;

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
         VertexId first_vertex_id,
         VertexId second_vertex_id,
         EdgeColor color)
        : id_(id),
          first_vertex_id_(first_vertex_id),
          second_vertex_id_(second_vertex_id),
          color_(color) {}
    EdgeId id() const override { return id_; }
    VertexId get_first_vertex_id() const override { return first_vertex_id_; }
    VertexId get_second_vertex_id() const override { return second_vertex_id_; }
    EdgeColor color() const override { return color_; }

   private:
    EdgeId id_ = 0;
    VertexId first_vertex_id_ = 0;
    VertexId second_vertex_id_ = 0;
    EdgeColor color_ = EdgeColor::Grey;
  };

  VertexId get_new_vertex_id() override { return last_vertex_id_++; }

  EdgeId get_new_edge_id() override { return last_edge_id_++; };

  bool has_vertex_id(VertexId vertex_id) const {
    return vertices_.find(vertex_id) != vertices_.end();
  }

  void set_vertex_depth(VertexId vertex_id, GraphDepth vertex_depth);

  void update_vertex_depth(VertexId vertex_id, GraphDepth vertex_depth);

  const EdgeColor get_edge_color(VertexId first_vertex_id,
                                 VertexId second_vertex_id) const;

  std::unordered_map<VertexId, std::unique_ptr<IVertex>> vertices_ = {};
  std::unordered_map<EdgeId, std::unique_ptr<IEdge>> edges_ = {};

  std::unordered_map<VertexId, std::vector<EdgeId>> connections_ = {};

  std::vector<std::vector<VertexId>> depth_levels_ = {};

  std::unordered_map<VertexId, GraphDepth> vertices_depth_ = {};

  std::unordered_map<EdgeColor, std::vector<EdgeId>> colored_edge_ids_ = {
      {EdgeColor::Grey, {}},
      {EdgeColor::Yellow, {}},
      {EdgeColor::Green, {}},
      {EdgeColor::Red, {}}};

  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
};
};  // namespace uni_course_cpp
