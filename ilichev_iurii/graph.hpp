#include <iostream>
#include <map>
#include <set>
#include <utility>

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;

  struct Vertex;
  struct Edge;

  void add_vertex();
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);
  void print_graph_info();

 private:
  std::map<Vertex, std::set<Edge>> vertex_map;
  EdgeId _next_edge_id = 0;
};
