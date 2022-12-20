#pragma once
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
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
  Graph::VertexId add_grey_edge(Graph& graph,
                                Graph::Depth current_depth,
                                Graph::VertexId vertex_id,
                                std::mutex& graph_mutex) const;
  void generate_grey_edges(Graph& graph,
                           std::mutex& graph_mutex,
                           Graph::VertexId root_vertex_id) const;
  void generate_grey_branch(Graph& graph,
                            std::mutex& graph_mutex,
                            Graph::VertexId root_vertex_id,
                            Graph::Depth current_depth) const;
  Params params_ = Params(0, 0);
};

}  // namespace uni_course_cpp
