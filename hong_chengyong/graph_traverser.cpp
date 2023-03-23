#include "graph_traverser.hpp"
#include <atomic>
#include <cassert>
#include <climits>
#include <functional>
#include <list>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>
#include "graph.hpp"

namespace {
constexpr int MAX_DISTANCE = INT_MAX;
const int MAX_THREADS_COUNT = std::thread::hardware_concurrency();
}  // namespace

namespace uni_course_cpp {

GraphPath GraphTraverser::findShortestPath(
    const VertexId& source_vertex_id,
    const VertexId& destination_vertex_id) const {
  assert(graph_.hasVertex(source_vertex_id));
  assert(graph_.hasVertex(destination_vertex_id));
  std::unordered_map<VertexId, GraphPath> all_paths;
  std::unordered_map<VertexId, GraphPath::Distance> distances;
  for (const auto& vertex : graph_.vertexes()) {
    distances[vertex.id] = MAX_DISTANCE;
  }
  all_paths[source_vertex_id].vertex_ids.push_back(source_vertex_id);
  std::queue<VertexId> pass_waiting;
  pass_waiting.push(source_vertex_id);
  while (!pass_waiting.empty()) {
    const auto current_vertex_id = pass_waiting.front();
    pass_waiting.pop();
    for (const auto& edge_id : graph_.vertexConnections(current_vertex_id)) {
      const auto edge = graph_.getEdge(edge_id);
      VertexId next_vertex_id =
          edge.vertex_id1 + edge.vertex_id2 - current_vertex_id;
      if (distances[current_vertex_id] + 1 < distances[next_vertex_id]) {
        pass_waiting.push(next_vertex_id);
        distances[next_vertex_id] = distances[current_vertex_id] + 1;
        all_paths[next_vertex_id] = all_paths[current_vertex_id];
        all_paths[next_vertex_id].vertex_ids.push_back(next_vertex_id);
        all_paths[next_vertex_id].edge_ids.push_back(edge_id);
      }
    }
  }
  return all_paths[destination_vertex_id];
}

std::vector<GraphPath> GraphTraverser::findAllPaths() const {
  std::vector<GraphPath> paths;
  using JobCallback = std::function<void()>;
  auto jobs = std::list<JobCallback>();
  std::mutex mutex;
  std::atomic<int> finished_paths = 0;
  std::atomic<bool> should_terminate = false;
  const VertexId START_VERTEX_ID = 0;
  std::vector<VertexId> finish_vertex_ids =
      graph_.vertexIdsAtLayer(graph_.depth());
  for (int i = 0; i < finish_vertex_ids.size(); i++) {
    const VertexId end_vertex_id = finish_vertex_ids[i];
    jobs.push_back([&START_VERTEX_ID, end_vertex_id, &paths, &mutex,
                    &finished_paths, this]() {
      const GraphPath new_path =
          findShortestPath(START_VERTEX_ID, end_vertex_id);
      {
        const std::lock_guard lock(mutex);
        paths.push_back(std::move(new_path));
      }
      finished_paths++;
    });
  }
  const auto worker = [&should_terminate, &mutex, &jobs]() {
    while (true) {
      if (should_terminate) {
        return;
      }
      const auto job_optional = [&jobs,
                                 &mutex]() -> std::optional<JobCallback> {
        const std::lock_guard lock(mutex);
        if (!jobs.empty()) {
          const auto job = jobs.front();
          jobs.pop_front();
          return job;
        }
        return std::nullopt;
      }();
      if (job_optional.has_value()) {
        const auto& job = job_optional.value();
        job();
      }
    }
  };

  const auto threads_num = std::min(
      MAX_THREADS_COUNT,
      static_cast<int>(graph_.vertexIdsAtLayer(graph_.depth()).size()));
  auto threads = std::vector<std::thread>();
  threads.reserve(threads_num);

  for (int i = 0; i < threads_num; i++) {
    threads.push_back(std::thread(worker));
  }

  while (finished_paths < finish_vertex_ids.size()) {
  }

  should_terminate = true;
  for (auto& thread : threads) {
    thread.join();
  }
  return paths;
}
}  // namespace uni_course_cpp
