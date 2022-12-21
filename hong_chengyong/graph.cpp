#include "graph.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {

bool Graph::hasVertex(const VertexId& vertex_id) const {
  for (const auto& vertex : vertexes_) {
    if (vertex.id == vertex_id) {
      return true;
    }
  }
  return false;
}

bool Graph::hasEdge(const EdgeId& edge_id) const {
  for (const auto& edge : edges_) {
    if (edge.id == edge_id) {
      return true;
    }
  }
  return false;
}

bool Graph::areConnected(const VertexId& vertex_id1,
                         const VertexId& vertex_id2) const {
  assert(hasVertex(vertex_id1) && "Vertex1 index is out of range");
  assert(hasVertex(vertex_id2) && "Vertex2 index is out of range");
  if (vertex_id1 == vertex_id2) {
    for (const auto& edge_id : connection_list_.at(vertex_id1)) {
      const auto& edge = getEdge(edge_id);
      if (edge.vertex_id1 == vertex_id1 && edge.vertex_id2 == vertex_id1) {
        return true;
      }
    }
  } else {
    for (const auto& edge_id : connection_list_.at(vertex_id1)) {
      const auto& edge = getEdge(edge_id);
      if (edge.vertex_id1 == vertex_id2 || edge.vertex_id2 == vertex_id2) {
        return true;
      }
    }
  }
  return false;
}

VertexId Graph::addVertex() {
  const auto& new_vertex = vertexes_.emplace_back(getNewVertexId());
  connection_list_.insert({new_vertex.id, std::vector<EdgeId>()});
  layers_list_[0].push_back(new_vertex.id);
  vertexes_depths_[new_vertex.id] = 0;
  return new_vertex.id;
}

Edge::Colors Graph::calculateEdgeColor(const VertexId& vertex_id1,
                                       const VertexId& vertex_id2) const {
  if (connection_list_.at(vertex_id2).size() == 0 ||
      connection_list_.at(vertex_id1).size() == 0) {
    return Edge::Colors::Gray;
  } else if (vertex_id1 == vertex_id2) {
    return Edge::Colors::Green;
  } else if (std::abs(vertexDepth(vertex_id2) - vertexDepth(vertex_id1)) == 1) {
    return Edge::Colors::Yellow;
  } else {
    return Edge::Colors::Red;
  }
}

void Graph::grayEdgeInitialization(const VertexId& vertex_id1,
                                   const VertexId& vertex_id2) {
  int new_depth = vertexes_depths_[std::min(vertex_id1, vertex_id2)] + 1;
  vertexes_depths_[std::max(vertex_id1, vertex_id2)] = new_depth;
  depth_ = std::max(depth_, new_depth);
  layers_list_[new_depth].push_back(std::max(vertex_id1, vertex_id2));
  for (auto it = layers_list_[0].begin(); it != layers_list_[0].end(); it++) {
    if (*it == std::max(vertex_id1, vertex_id2)) {
      layers_list_[0].erase(it);
      break;
    }
  }
}

void Graph::addEdge(const VertexId& vertex_id1, const VertexId& vertex_id2) {
  assert(hasVertex(vertex_id1) && "Vertex1 index is out of range");
  assert(hasVertex(vertex_id2) && "Vertex2 index is out of range");
  assert(!areConnected(vertex_id1, vertex_id2) &&
         "These vertexes are already connected");
  auto color = calculateEdgeColor(vertex_id1, vertex_id2);
  if (color == Edge::Colors::Gray) {
    grayEdgeInitialization(vertex_id1, vertex_id2);
  }
  const auto& new_edge =
      edges_.emplace_back(getNewEdgeId(), vertex_id1, vertex_id2, color);
  color_list_[color].push_back(new_edge.id);
  connection_list_[vertex_id1].push_back(new_edge.id);
  if (vertex_id1 != vertex_id2) {
    connection_list_[vertex_id2].push_back(new_edge.id);
  }
}

const Edge& Graph::getEdge(const EdgeId& edge_id) const {
  assert(hasEdge(edge_id) && "Edge id is out of range.");
  for (const auto& edge : edges_) {
    if (edge.id == edge_id) {
      return edge;
    }
  }
  throw std::runtime_error("Cannot be reached.");
}

const std::vector<EdgeId>& Graph::vertexConnections(const VertexId& id) const {
  assert(hasVertex(id) && "Vertex id is out of range");
  return connection_list_.at(id);
}
const std::vector<VertexId>& Graph::vertexIdsAtLayer(int depth) const {
  assert(depth <= depth_ && "Graph is not that deep");
  return layers_list_.at(depth);
}
int Graph::vertexDepth(const VertexId& vertex_id) const {
  assert(hasVertex(vertex_id) && "Vertex id is out of range");
  return vertexes_depths_.at(vertex_id);
}
const std::vector<EdgeId>& Graph::colorEdges(const Edge::Colors& color) const {
  if (color_list_.find(color) == color_list_.end()) {
    static std::vector<EdgeId> empty_vector;
    return empty_vector;
  }
  return color_list_.at(color);
}
}  // namespace uni_course_cpp