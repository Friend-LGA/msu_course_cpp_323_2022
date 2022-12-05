#include "graph_generator.hpp"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <deque>
#include <optional>
#include <random>
#include <thread>

namespace uni_course_cpp {

namespace {
bool check_probabilty(float probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution p(probability);

  return p(gen);
}

int get_random_vertex_id(const std::set<Graph::VertexId>& vertex_ids) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, vertex_ids.size() - 1);
  return *std::next(vertex_ids.begin(), distrib(gen));
}

std::set<Graph::VertexId> get_unconnected_vertex_ids(
    const Graph& graph,
    Graph::Depth current_depth,
    Graph::VertexId vertex_id) {
  const std::set<Graph::VertexId>& deeper_vertices =
      graph.vertices_at_depth(current_depth + kYellowDepthDifference);
  const std::set<Graph::VertexId> children_vertices =
      graph.children_of_vertex(vertex_id);
  std::set<Graph::VertexId> unconnected_vertices;
  std::set_difference(
      deeper_vertices.begin(), deeper_vertices.end(), children_vertices.begin(),
      children_vertices.end(),
      std::inserter(unconnected_vertices, unconnected_vertices.begin()));
  return unconnected_vertices;
}

void generate_green_edges(Graph& graph, std::mutex& graph_mutex) {
  constexpr float probabilty_of_green_edge = 0.1;
  const auto& vertices = graph.vertices();
  std::for_each(vertices.begin(), vertices.end(),
                [&graph, &graph_mutex](
                    const std::pair<Graph::VertexId, Graph::Vertex>& element) {
                  Graph::VertexId vertex_id = element.first;
                  if (check_probabilty(probabilty_of_green_edge)) {
                    const std::lock_guard graph_guard(graph_mutex);
                    graph.add_edge(vertex_id, vertex_id);
                  }
                });
}

void generate_red_edges(Graph& graph, std::mutex& graph_mutex) {
  constexpr float probabilty_of_red_edge = 0.33;
  for (Graph::Depth current_depth = kGraphBaseDepth;
       current_depth <= graph.depth() - kRedDepthDifference; ++current_depth) {
    const auto& vertices = graph.vertices_at_depth(current_depth);
    std::for_each(
        vertices.begin(), vertices.end(),
        [&graph, &graph_mutex, current_depth](const Graph::VertexId vertex_id) {
          if (check_probabilty(probabilty_of_red_edge)) {
            const std::set<Graph::VertexId>& deeper_vertices =
                graph.vertices_at_depth(current_depth + kRedDepthDifference);
            if (deeper_vertices.size() > 0) {
              const Graph::VertexId random_vertex_id =
                  get_random_vertex_id(deeper_vertices);
              const std::lock_guard graph_guard(graph_mutex);
              graph.add_edge(vertex_id, random_vertex_id);
            }
          }
        });
  }
}

void generate_yellow_edges(Graph& graph, std::mutex& graph_mutex) {
  for (Graph::Depth current_depth = kGraphBaseDepth;
       current_depth <= graph.depth() - kYellowDepthDifference;
       ++current_depth) {
    const auto& vertices = graph.vertices_at_depth(current_depth);
    std::for_each(
        vertices.begin(), vertices.end(),
        [&graph, &graph_mutex, current_depth](const Graph::VertexId vertex_id) {
          const float probabilty_of_yellow_edge =
              static_cast<float>((current_depth - kYellowDepthDifference)) /
              (graph.depth() - kYellowDepthDifference - kGraphBaseDepth);
          if (check_probabilty(probabilty_of_yellow_edge)) {
            const std::lock_guard graph_guard(graph_mutex);
            std::set<Graph::VertexId> unconnected_vertices =
                get_unconnected_vertex_ids(graph, current_depth, vertex_id);
            if (unconnected_vertices.size() > 0) {
              const Graph::VertexId random_vertex_id =
                  get_random_vertex_id(unconnected_vertices);
              graph.add_edge(vertex_id, random_vertex_id);
            }
          }
        });
  }
}

}  // namespace

void GraphGenerator::generate_branch(Graph& graph,
                                     Graph::VertexId root_id,
                                     Graph::Depth root_depth,
                                     std::mutex& graph_mutex) const {
  if (root_depth == params_.depth()) {
    return;
  }
  assert(root_depth < params_.depth());
  const float probabilty_of_grey_edge =
      1.0 - static_cast<float>((root_depth - kGreyDepthDifference)) /
                (params_.depth() - kGreyDepthDifference);
  const Graph::VertexId branch_root_id = [&graph, &graph_mutex, root_id]() {
    const std::lock_guard graph_guard(graph_mutex);
    const Graph::VertexId child_id = graph.add_vertex();
    graph.add_edge(root_id, child_id);
    return child_id;
  }();
  for (int i = 0; i < params_.new_vertices_count(); ++i) {
    if (check_probabilty(probabilty_of_grey_edge)) {
      generate_branch(graph, branch_root_id, root_depth + 1, graph_mutex);
    }
  }
}

using JobCallback = std::function<void()>;
const int kMaxThreadsCount = std::thread::hardware_concurrency();

void GraphGenerator::generate_new_vertices(Graph& graph,
                                           Graph::VertexId root_id) const {
  std::deque<JobCallback> tasks;
  std::mutex graph_mutex;
  std::atomic<int> tasks_count = params_.new_vertices_count();
  for (int i = 0; i < params_.new_vertices_count(); ++i) {
    tasks.push_front([&graph, &graph_mutex, root_id, this]() {
      generate_branch(graph, root_id, kGraphBaseDepth, graph_mutex);
    });
  }

  const int number_of_threads =
      std::min(params_.new_vertices_count(), kMaxThreadsCount);
  std::vector<std::thread> workers;
  std::mutex tasks_mutex;
  std::atomic<bool> should_terminate = false;
  workers.reserve(number_of_threads);
  for (int i = 0; i < number_of_threads; ++i) {
    workers.emplace_back(
        [&tasks_mutex, &tasks, &should_terminate, &tasks_count]() {
          while (!should_terminate) {
            const auto task_optional =
                [&tasks, &tasks_mutex]() -> std::optional<JobCallback> {
              const std::lock_guard graph_guard(tasks_mutex);
              if (!tasks.empty()) {
                auto task = tasks.back();
                tasks.pop_back();
                return task;
              }
              return std::nullopt;
            }();
            if (task_optional.has_value()) {
              const auto& task = task_optional.value();
              task();
              tasks_count -= 1;
            }
          }
        });
  }

  while (tasks_count != 0) {
  }
  should_terminate = true;
  for (auto& worker : workers) {
    worker.join();
  }
}

Graph GraphGenerator::generate() const {
  auto graph = Graph();
  if (params_.depth() > 0) {
    const Graph::VertexId root_id = graph.add_vertex();
    generate_new_vertices(graph, root_id);
  }
  std::mutex graph_mutex;
  auto green_thread = std::thread(
      [&graph, &graph_mutex]() { generate_green_edges(graph, graph_mutex); });
  auto red_thread = std::thread(
      [&graph, &graph_mutex]() { generate_red_edges(graph, graph_mutex); });
  auto yellow_thread = std::thread(
      [&graph, &graph_mutex]() { generate_yellow_edges(graph, graph_mutex); });
  green_thread.join();
  red_thread.join();
  yellow_thread.join();

  return graph;
}
}  // namespace uni_course_cpp
