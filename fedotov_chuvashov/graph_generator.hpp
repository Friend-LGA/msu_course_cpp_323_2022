#pragma once
#include <mutex>
#include "graph.hpp"

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
  void generate_branch(Graph& graph,
                       Graph::VertexId root_id,
                       Graph::Depth root_depth,
                       std::mutex& mutex) const;

  void generate_new_vertices(Graph& graph, Graph::VertexId root_id) const;
  Params params_ = Params(0, 0);
};
}  // namespace uni_course_cpp
