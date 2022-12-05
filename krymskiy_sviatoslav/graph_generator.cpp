#include <cassert>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "graph_generator.hpp"
#include "printing.hpp"

constexpr int kVerticesCount = 14;

Graph::VertexId Graph::get_new_vertex_id() {
  return current_vertex_id_++;
}

Graph::EdgeId Graph::get_new_edge_id() {
  return current_edge_id_++;
}

bool Graph::has_vertex(VertexId id) const {
  return vertices_.find(id) != vertices_.end();
}

void Graph::add_vertex() {
  const VertexId id = get_new_vertex_id();
  vertices_.emplace(id, id);
}

void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  assert(has_vertex(from_vertex_id));
  assert(has_vertex(to_vertex_id));
  const EdgeId id = get_new_edge_id();
  edges_.try_emplace(id, id, from_vertex_id, to_vertex_id);
  adjacency_list_[from_vertex_id].emplace_back(id);
  if (from_vertex_id != to_vertex_id) {
    adjacency_list_[to_vertex_id].emplace_back(id);
  }
}

const std::unordered_map<Graph::VertexId, Graph::Vertex>& Graph::get_vertices()
    const {
  return vertices_;
}

const std::unordered_map<Graph::EdgeId, Graph::Edge>& Graph::get_edges() const {
  return edges_;
}

const std::vector<Graph::EdgeId>& Graph::get_vertex_edge_ids(
    Graph::VertexId id) const {
  return adjacency_list_.at(id);
}

Graph generate_graph() {
  auto graph = Graph();
  for (int i = 0; i < kVerticesCount; i++) {
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

  return graph;
}

void write_to_file(const std::string& graph_json,
                   const std::string& file_name) {
  std::ofstream output(file_name);
  output << graph_json;
}

int main() {
  const auto graph = generate_graph();
  const auto graph_json = printing::json::print_graph(graph);
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");
  return 0;
}
