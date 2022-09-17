#include <iostream>
#include <map>
#include <set>

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;

  void add_vertex();
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);
  void print_graph_info();

 private:
  struct Vertex;
  struct Edge;
  std::map<Vertex, std::set<Edge>> verticies_map;
  std::set<Edge> edges;
};
