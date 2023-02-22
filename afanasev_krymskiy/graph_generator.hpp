#pragma once

#include <memory>
#include <mutex>

#include "graph.hpp"
#include "interfaces/i_graph.hpp"
#include "interfaces/i_vertex.hpp"

namespace uni_course_cpp {
class GraphGenerator {
 public:
  struct Params {
   public:
    Params(GraphDepth depth, int new_vertices_count)
        : depth_(depth), new_vertices_count_(new_vertices_count) {}

    GraphDepth get_depth() const { return depth_; }
    int new_vertices_count() const { return new_vertices_count_; }

   private:
    GraphDepth depth_ = 0;
    int new_vertices_count_ = 0;
  };

  explicit GraphGenerator(Params&& params) : params_(std::move(params)) {}

  std::unique_ptr<IGraph> generate() const;
  void generate_grey_branch(Graph&, std::mutex&, VertexId, GraphDepth) const;

 private:
  void generate_grey_edges(Graph& graph, VertexId root_vertex_id) const;

  Params params_ = Params(0, 0);
};
}  // namespace uni_course_cpp
