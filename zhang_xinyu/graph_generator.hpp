#include "graph.hpp"
#pragma once

class GraphGenerator {
 public:
  struct Params {
   public:
    Params(Graph::Depth depth, int new_vertices_count) :
      depth_(depth), new_vertices_count_(new_vertices_count) {}

    Graph::Depth depth() const { return depth_; }
    int new_vertices_count() const { return new_vertices_count_; }

   private:
    Graph::Depth depth_ = 0;
    int new_vertices_count_ = 0;
  };

  explicit GraphGenerator(Params&& params) : params_(std::move(params)) {}

    Graph generate() const;

 private:
    void generate_grey_edges(Graph& graph) const;
    void generate_green_edges(Graph& graph) const;
    void generate_yellow_edges(Graph& graph) const;
    void generate_red_edges(Graph& graph) const;
  Params params_ = Params(0, 0);
};




