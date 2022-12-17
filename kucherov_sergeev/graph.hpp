#pragma once

#include <unordered_map>
#include <vector>

#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {
class Graph : public IGraph {
 public:
  VertexId add_vertex() override;

  EdgeId add_edge(VertexId from_vertex_id, VertexId to_vertex_id) override;

  GraphDepth get_depth() const override;

  const std::vector<VertexId>& get_depth_vertex_ids(
      GraphDepth depth) const override;

  const std::vector<EdgeId>& get_connected_edge_ids(
      VertexId vertex_id) const override;

  bool is_vertices_connected(VertexId first_vertex_id,
                             VertexId second_vertex_id) const override;

  GraphDepth get_vertex_depth(VertexId vertex_id) const override;

  const std::unordered_map<VertexId, std::unique_ptr<IVertex>>& get_vertices()
      const override;

  const std::unordered_map<EdgeId, std::unique_ptr<IEdge>>& get_edges()
      const override;

 private:
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

  struct Vertex : public IVertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const override { return id_; }

   private:
    VertexId id_ = 0;
  };

  VertexId get_new_vertex_id();

  EdgeId get_new_edge_id();

  EdgeColor determine_edge_color(VertexId from_vertex_id,
                                 VertexId to_vertex_id) const;

  void set_vertex_depth(VertexId vertex_id, GraphDepth depth);

  VertexId next_free_vertex_id_ = 0;
  EdgeId next_free_edge_id_ = 0;
  std::unordered_map<VertexId, std::unique_ptr<IVertex>> vertices_;
  std::unordered_map<EdgeId, std::unique_ptr<IEdge>> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;
  std::unordered_map<VertexId, GraphDepth> vertex_depths_list_;
  std::vector<std::vector<VertexId>> depth_vertices_list_ = {{}};
};

static constexpr GraphDepth kGraphDefaultDepth = 1;

}  // namespace uni_course_cpp
