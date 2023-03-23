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

bool Graph::areConnected(const VertexId& from_vertex_id,
                         const VertexId& to_vertex_id) const {
  assert(hasVertex(from_vertex_id) && "Vertex1 index is out of range");
  assert(hasVertex(to_vertex_id) && "Vertex2 index is out of range");
  if (from_vertex_id == to_vertex_id) {
    for (const auto& edge_id : connection_list_.at(from_vertex_id)) {
      const auto& edge = getEdge(edge_id);
      if (edge.from_vertex_id == from_vertex_id &&
          edge.to_vertex_id == from_vertex_id) {
        return true;
      }
    }
  } else {
    for (const auto& edge_id : connection_list_.at(from_vertex_id)) {
      const auto& edge = getEdge(edge_id);
      if (edge.from_vertex_id == to_vertex_id ||
          edge.to_vertex_id == to_vertex_id) {
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

Edge::Colors Graph::calculateEdgeColor(const VertexId& from_vertex_id,
                                       const VertexId& to_vertex_id) const {
  if (connection_list_.at(to_vertex_id).size() == 0 ||
      connection_list_.at(from_vertex_id).size() == 0) {
    return Edge::Colors::Gray;
  } else if (from_vertex_id == to_vertex_id) {
    return Edge::Colors::Green;
  } else if (std::abs(vertexDepth(to_vertex_id) -
                      vertexDepth(from_vertex_id)) == 1) {
    return Edge::Colors::Yellow;
  } else {
    return Edge::Colors::Red;
  }
}

void Graph::grayEdgeInitialization(const VertexId& from_vertex_id,
                                   const VertexId& to_vertex_id) {
  int new_depth = vertexes_depths_[std::min(from_vertex_id, to_vertex_id)] + 1;
  vertexes_depths_[std::max(from_vertex_id, to_vertex_id)] = new_depth;
  depth_ = std::max(depth_, new_depth);
  layers_list_[new_depth].push_back(std::max(from_vertex_id, to_vertex_id));
  for (auto it = layers_list_[0].begin(); it != layers_list_[0].end(); it++) {
    if (*it == std::max(from_vertex_id, to_vertex_id)) {
      layers_list_[0].erase(it);
      break;
    }
  }
}

void Graph::addEdge(const VertexId& from_vertex_id,
                    const VertexId& to_vertex_id) {
  assert(hasVertex(from_vertex_id) && "Vertex1 index is out of range");
  assert(hasVertex(to_vertex_id) && "Vertex2 index is out of range");
  assert(!areConnected(from_vertex_id, to_vertex_id) &&
         "These vertexes are already connected");
  auto color = calculateEdgeColor(from_vertex_id, to_vertex_id);
  if (color == Edge::Colors::Gray) {
    grayEdgeInitialization(from_vertex_id, to_vertex_id);
  }
  const auto& new_edge =
      edges_.emplace_back(getNewEdgeId(), from_vertex_id, to_vertex_id, color);
  color_list_[color].push_back(new_edge.id);
  connection_list_[from_vertex_id].push_back(new_edge.id);
  if (from_vertex_id != to_vertex_id) {
    connection_list_[to_vertex_id].push_back(new_edge.id);
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
const std::vector<VertexId>& Graph::vertexIdsAtLayer(
    uni_course_cpp::Depth depth) const {
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
