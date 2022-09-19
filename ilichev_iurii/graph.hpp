#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <list>
#include <map>
#include <vector>

// An undirected graph, there can be only one edge between any two vertices
class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;

  struct Edge {
   public:
    Edge();
    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id);

    EdgeId id() const;
    VertexId from_vertex_id() const;
    VertexId to_vertex_id() const;

   private:
    EdgeId id_;
    VertexId from_vertex_id_;
    VertexId to_vertex_id_;
  };

  struct Vertex {
   public:
    explicit Vertex(VertexId id);

    VertexId id() const;

   private:
    VertexId id_;
  };

  void add_vertex();
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);
  void print_graph_info();
  bool edge_correctness(VertexId from_vertex_id, VertexId to_vertex_id);

 private:
  std::vector<Vertex> vertexes_;
  std::vector<Edge> edges_;
  std::map<VertexId, std::list<EdgeId>> vertex_edges_;
};

#endif
