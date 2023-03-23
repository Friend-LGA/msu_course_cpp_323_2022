#pragma once
#include <mutex>
#include "graph.hpp"

namespace uni_course_cpp {

class GraphGenerator {
 public:
  struct Params {
   public:
    Params(Graph::Depth depth, int new_vertexes_count)
        : depth_(depth), new_vertexes_count_(new_vertexes_count) {}

    Graph::Depth depth() const { return depth_; }
    int new_vertexes_count() const { return new_vertexes_count_; }

   private:
    Graph::Depth depth_ = 0;
    int new_vertexes_count_ = 0;
  };

  explicit GraphGenerator(Params&& params) : params_(std::move(params)) {}

  Graph generate() const;

 private:
  void generate_grey_edges(Graph& graph,
                           Graph::VertexId root_id,
                           std::mutex& graph_mutex) const;
  void generate_grey_branch(Graph& graph,
                            std::mutex& graph_mutex,
                            Graph::VertexId root_id,
                            Graph::Depth cur_depth) const;

  Params params_ = Params(0, 0);
};

}  // namespace uni_course_cpp
