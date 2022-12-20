#pragma once

#include <vector>
#include "interfaces/i_graph.hpp"
#include "interfaces/i_graph_path.hpp"

namespace uni_course_cpp {

class GraphTraverser {
 public:
  struct GraphPath : public IGraphPath {
    GraphPath(std::vector<VertexId> _vertex_ids, std::vector<EdgeId> _edge_ids)
        : vertex_ids_(_vertex_ids), edge_ids_(_edge_ids) {}

    PathDistance distance() const override { return vertex_ids_.size(); }
    const std::vector<VertexId>& vertex_ids() const override {
      return vertex_ids_;
    }
    const std::vector<EdgeId>& edge_ids() const override { return edge_ids_; }

   private:
    std::vector<VertexId> vertex_ids_;
    std::vector<EdgeId> edge_ids_;
  };

  GraphTraverser(const IGraph& graph) : graph_(graph) {}

  GraphPath find_shortest_path(VertexId source_vertex_id,
                               VertexId destination_vertex_id) const;

  std::vector<GraphPath> find_all_paths() const;

 private:
  const IGraph& graph_;
};

}  // namespace uni_course_cpp
