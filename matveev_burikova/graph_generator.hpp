#pragma once

#include "graph.hpp"

#include <mutex>

namespace uni_course_cpp {
class GraphGenerator {
 public:
  struct Params {
   public:
    Params(Graph::Depth depth, int new_vertices_count)
        : depth_(depth), new_vertices_count_(new_vertices_count) {}

    Graph::Depth depth() const { return depth_; }
    int new_vertices_count() const { return new_vertices_count_; }

   private:
    Graph::Depth depth_ = 0;
    int new_vertices_count_ = 0;
  };

  explicit GraphGenerator(Params&& params) : params_(std::move(params)) {}
  Graph generate() const;

 private:
  Params params_ = Params(0, 0);

  void generate_grey_edges(Graph& graph, Graph::VertexId base_vertex_id) const;

  void generate_grey_branch(Graph& graph,
                            std::mutex& graph_mutex,
                            Graph::VertexId from_vertex_id,
                            Graph::Depth current_depth) const;
};
}  // namespace uni_course_cpp
