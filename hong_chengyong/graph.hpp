#pragma once

#include <array>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {

using VertexId = int;
using EdgeId = int;

struct Vertex {
  const VertexId id;

  explicit Vertex(const VertexId& _id) : id(_id) {}
};

struct Edge {
  enum class Colors { Gray, Green, Yellow, Red };
  static constexpr std::array<Edge::Colors, 4> ALL_COLORS = {
      Colors::Gray, Colors::Green, Colors::Yellow, Colors::Red};
  const EdgeId id;
  const VertexId vertex_id1;
  const VertexId vertex_id2;
  const Colors color;
  Edge(const EdgeId& _id,
       const VertexId& _vertex_id1,
       const VertexId& _vertex_id2,
       const Colors& _color)
      : id(_id),
        vertex_id1(_vertex_id1),
        vertex_id2(_vertex_id2),
        color(_color) {}
};

class Graph {
 public:
  bool hasVertex(const VertexId& vertex_id) const;

  bool hasEdge(const EdgeId& edge_id) const;

  bool areConnected(const VertexId& vertex_id1,
                    const VertexId& vertex_id2) const;

  VertexId addVertex();

  void addEdge(const VertexId& vertex_id1, const VertexId& vertex_id2);

  const Edge& getEdge(const EdgeId& edge_id) const;

  const std::vector<EdgeId>& vertexConnections(const VertexId& id) const;
  const std::vector<Vertex>& vertexes() const { return vertexes_; }
  const std::vector<Edge>& edges() const { return edges_; }
  const std::vector<VertexId>& vertexIdsAtLayer(int depth) const;
  int vertexDepth(const VertexId& vertex_id) const;
  int depth() const { return depth_; }
  const std::vector<EdgeId>& colorEdges(const Edge::Colors& color) const;

 private:
  std::vector<Vertex> vertexes_;
  std::vector<Edge> edges_;
  int depth_ = 0;
  int vertex_new_id_ = 0, edge_new_id_ = 0;
  std::unordered_map<VertexId, std::vector<EdgeId>> connection_list_;
  std::unordered_map<int, std::vector<VertexId>> layers_list_;
  std::unordered_map<VertexId, int> vertexes_depths_;
  std::unordered_map<Edge::Colors, std::vector<EdgeId>> color_list_;
  VertexId getNewVertexId() { return vertex_new_id_++; }
  EdgeId getNewEdgeId() { return edge_new_id_++; }
  Edge::Colors calculateEdgeColor(const VertexId& vertex_id1,
                                  const VertexId& vertex_id2) const;
  void grayEdgeInitialization(const VertexId& vertex_id1,
                              const VertexId& vertex_id2);
};
}  // namespace uni_course_cpp