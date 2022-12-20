#include "graph_traverser.hpp"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <thread>
#include <utility>
#include <vector>
#include "graph.hpp"

namespace {
constexpr int MAX_DISTANCE = 100000;
constexpr int kBaseDistance = 1;
const unsigned long MAX_WORKERS_COUNT = std::thread::hardware_concurrency();
static const unsigned long kMaxThreadsCount =
    std::thread::hardware_concurrency();
}  // namespace

namespace uni_course_cpp {

GraphTraverser::GraphPath GraphTraverser::find_shortest_path(
    VertexId source_vertex_id,
    VertexId destination_vertex_id) const {
  std::queue<VertexId> vertices_id_queue;
  vertices_id_queue.push(source_vertex_id);

  const auto vertices_number = graph_.get_vertices().size();

  std::vector<PathDistance> distances(vertices_number, MAX_DISTANCE);
  std::vector<VertexId> from_vertices_id(vertices_number);
  std::vector<VertexId> from_edge_id(vertices_number);
  distances[source_vertex_id] = 0;

  while (!vertices_id_queue.empty()) {
    const auto current_vertex_id = vertices_id_queue.front();
    vertices_id_queue.pop();
    for (const auto edge_id : graph_.connected_edges_ids(current_vertex_id)) {
      const auto pair_of_vertex_ids =
          graph_.get_vertex_ids_from_edge_id(edge_id);
      VertexId next_vertex_id;
      if (pair_of_vertex_ids.first != current_vertex_id) {
        next_vertex_id = pair_of_vertex_ids.first;
      } else {
        next_vertex_id = pair_of_vertex_ids.second;
      }
      if (distances[next_vertex_id] >
          distances[current_vertex_id] + kBaseDistance) {
        distances[next_vertex_id] =
            distances[current_vertex_id] + kBaseDistance;
        vertices_id_queue.push(next_vertex_id);
        from_vertices_id[next_vertex_id] = current_vertex_id;
        from_edge_id[next_vertex_id] = edge_id;
      }
    }
  }

  std::vector<VertexId> vertex_path;
  std::vector<EdgeId> edge_path;

  VertexId current_vertex_id = destination_vertex_id;
  while (current_vertex_id != source_vertex_id) {
    vertex_path.emplace_back(current_vertex_id);
    edge_path.emplace_back(from_edge_id[current_vertex_id]);

    current_vertex_id = from_vertices_id[current_vertex_id];
  }
  vertex_path.emplace_back(source_vertex_id);

  std::reverse(vertex_path.begin(), vertex_path.end());
  std::reverse(edge_path.begin(), edge_path.end());

  return GraphPath(vertex_path, edge_path);
}

std::vector<GraphTraverser::GraphPath> GraphTraverser::find_all_paths() const {
  std::deque<std::function<void()>> jobs;
  std::atomic<int> completed_paths = 0;
  std::mutex path_mutex;

  const auto& vertex_ids = graph_.get_vertices_with_depth(graph_.depth());
  const auto& root_id =
      graph_.get_vertices_with_depth(Graph::kBaseDepth).back();

  std::vector<GraphTraverser::GraphPath> paths;
  paths.reserve(vertex_ids.size());

  for (const auto& vertex_id : vertex_ids)
    jobs.push_back([this, &graph_ = graph_, &completed_paths, &vertex_id,
                    &paths, &path_mutex, &root_id]() {
      auto path = find_shortest_path(root_id, vertex_id);
      {
        std::lock_guard lock(path_mutex);
        paths.emplace_back(path);
      }
      completed_paths++;
    });

  std::atomic<bool> should_terminate = false;
  std::mutex jobs_mutex;
  auto worker = [&should_terminate, &jobs_mutex, &jobs]() {
    while (true) {
      if (should_terminate) {
        return;
      }
      const auto job_optional =
          [&jobs_mutex, &jobs]() -> std::optional<std::function<void()>> {
        const std::lock_guard lock(jobs_mutex);
        if (jobs.empty()) {
          return std::nullopt;
        }
        const auto job = jobs.front();
        jobs.pop_front();
        return job;
      }();
      if (job_optional.has_value()) {
        const auto& job = job_optional.value();
        job();
      }
    }
  };

  const auto threads_number = std::min(vertex_ids.size(), kMaxThreadsCount);
  auto threads = std::vector<std::thread>();
  threads.reserve(threads_number);

  for (int i = 0; i < threads_number; ++i) {
    threads.emplace_back(worker);
  }

  while (completed_paths != vertex_ids.size()) {
  }

  should_terminate = true;
  for (auto& thread : threads) {
    thread.join();
  }

  return paths;
}

}  // namespace uni_course_cpp
