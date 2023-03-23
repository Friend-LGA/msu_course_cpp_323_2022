#pragma once

#include <array>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {

using VertexId = int;
using EdgeId = int;
using Depth = int;

struct Vertex {
  const VertexId id;

  explicit Vertex(const VertexId& _id) : id(_id) {}
};

struct Edge {
  enum class Colors { Gray, Green, Yellow, Red };
  static constexpr std::array<Edge::Colors, 4> ALL_COLORS = {
      Colors::Gray, Colors::Green, Colors::Yellow, Colors::Red};
  const EdgeId id;
  const VertexId from_vertex_id;
  const VertexId to_vertex_id;
  const Colors color;
  Edge(const EdgeId& _id,
       const VertexId& _from_vertex_id,
       const VertexId& _to_vertex_id,
       const Colors& _color)
      : id(_id),
        from_vertex_id(_from_vertex_id),
        to_vertex_id(_to_vertex_id),
        color(_color) {}
};

class Graph {
 public:
  bool hasVertex(const VertexId& vertex_id) const;

  bool hasEdge(const EdgeId& edge_id) const;

  bool areConnected(const VertexId& from_vertex_id,
                    const VertexId& to_vertex_id) const;

  VertexId addVertex();

  void addEdge(const VertexId& from_vertex_id, const VertexId& to_vertex_id);

  const Edge& getEdge(const EdgeId& edge_id) const;

  const std::vector<EdgeId>& vertexConnections(const VertexId& id) const;
  const std::vector<Vertex>& vertexes() const { return vertexes_; }
  const std::vector<Edge>& edges() const { return edges_; }
  const std::vector<VertexId>& vertexIdsAtLayer(Depth depth) const;
  int vertexDepth(const VertexId& vertex_id) const;
  Depth depth() const { return depth_; }
  const std::vector<EdgeId>& colorEdges(const Edge::Colors& color) const;

 private:
  std::vector<Vertex> vertexes_;
  std::vector<Edge> edges_;
  Depth depth_ = 0;
  int vertex_new_id_ = 0, edge_new_id_ = 0;
  std::unordered_map<VertexId, std::vector<EdgeId>> connection_list_;
  std::unordered_map<int, std::vector<VertexId>> layers_list_;
  std::unordered_map<VertexId, int> vertexes_depths_;
  std::unordered_map<Edge::Colors, std::vector<EdgeId>> color_list_;
  VertexId getNewVertexId() { return vertex_new_id_++; }
  EdgeId getNewEdgeId() { return edge_new_id_++; }
  Edge::Colors calculateEdgeColor(const VertexId& from_vertex_id,
                                  const VertexId& to_vertex_id) const;
  void grayEdgeInitialization(const VertexId& from_vertex_id,
                              const VertexId& to_vertex_id);
};
}  // namespace uni_course_cpp
