#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  void add_vertex();
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);
  VertexId get_lastVertexId() { return last_vertex_id_; }
  EdgeId get_lastEdgeId() { return last_edge_id_; }
  bool YorN_edgeId_for_vertexId(VertexId id, EdgeId j) {
    if (edges_[j].from_vertex_id() == id || edges_[j].to_vertex_id() == id)
      return true;
    else
      return false;
  }
  VertexId get_from_vertexid(EdgeId i) { return edges_[i].from_vertex_id(); }
  VertexId get_to_vertexid(EdgeId i) { return edges_[i].to_vertex_id(); }

 private:
  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }
    VertexId get_new_vertex_id() { return last_vertex_id_++; }

   private:
    VertexId id_ = 0;
    size_t last_vertex_id_ = 0;
  };

  struct Edge {
   public:
    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id) {}
    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; }
    VertexId to_vertex_id() const { return to_vertex_id_; }

   private:
    EdgeId id_;
    VertexId from_vertex_id_;
    VertexId to_vertex_id_;
  };

  VertexId get_new_vertex_id() { return last_vertex_id_++; }
  EdgeId get_new_edge_id() { return last_edge_id_++; }

  vector<Vertex> vertexes_;
  vector<Edge> edges_;
  VertexId last_vertex_id_ = 0;
  EdgeId last_edge_id_ = 0;
};
void Graph::add_vertex() {
  vertexes_.emplace_back(Graph::get_new_vertex_id());
}
void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  edges_.emplace_back(Graph::get_new_edge_id(), from_vertex_id, to_vertex_id);
}
constexpr int kVertexesCount = 14;
namespace printing {
namespace json {
const string print_graph(Graph graph) {
  string c = "{\n  \"vertices\": [\n";
  for (int i = 0; i < graph.get_lastVertexId(); i++) {
    c = c + "   {\"id\": ";
    c = c + to_string(i);
    c = c + ",\"edge_ids\": [";
    int j;
    for (j = 0; j < graph.get_lastEdgeId(); j++)
      if (graph.YorN_edgeId_for_vertexId(i, j)) {
        c += to_string(j);
        break;
      }
    for (; j < graph.get_lastEdgeId(); j++) {
      if (graph.YorN_edgeId_for_vertexId(i, j)) {
        c += ",";
        c += to_string(j);
      }
    }
    if (i != graph.get_lastVertexId() - 1)
      c += "]},\n";
    else
      c += "]}\n";
  }
  c += "  ]\n  \"edges\": [\n";

  for (int i = 0; i < graph.get_lastEdgeId(); i++) {
    c += "   {\"id\": ";
    c += to_string(i);
    c += ",\"vertex_ids\": [";
    c += to_string(graph.get_from_vertexid(i));
    c += ",";
    c += to_string(graph.get_to_vertexid(i));
    if (i != graph.get_lastEdgeId())
      c += "]},\n";
    else
      c += "]}\n";
  }
  c += "  ]\n}";
  return c;
}
}  // namespace json

}  // namespace printing
int main() {
  ofstream File;
  File.open("graph.json");
  Graph graph = Graph();
  for (int i = 0; i < kVertexesCount; i++) {
    graph.add_vertex();
  }
  graph.add_edge(0, 1);
  graph.add_edge(0, 2);
  graph.add_edge(0, 3);
  graph.add_edge(1, 4);
  graph.add_edge(1, 5);
  graph.add_edge(1, 6);
  graph.add_edge(2, 7);
  graph.add_edge(2, 8);
  graph.add_edge(3, 9);
  graph.add_edge(4, 10);
  graph.add_edge(5, 10);
  graph.add_edge(6, 10);
  graph.add_edge(7, 11);
  graph.add_edge(8, 11);
  graph.add_edge(9, 12);
  graph.add_edge(10, 13);
  graph.add_edge(11, 13);
  graph.add_edge(12, 13);
  const string graph_json = printing::json::print_graph(graph);
  cout << graph_json << endl;
  File << graph_json;
  File.close();
  return 0;
}
