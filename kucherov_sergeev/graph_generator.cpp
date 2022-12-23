#include <algorithm>
#include <atomic>
#include <cassert>
#include <functional>
#include <list>
#include <optional>
#include <random>
#include <thread>

#include "graph.hpp"
#include "graph_generator.hpp"

namespace uni_course_cpp {
namespace {
const int kMaxThreadsCount = std::thread::hardware_concurrency();
static constexpr float kEdgeGreenProbability = 0.1;
static constexpr float kEdgeRedProbability = 0.33;
static constexpr Graph::Depth kYellowEdgeLength = 1;
static constexpr Graph::Depth kRedEdgeLength = 2;

bool get_random_bool(float true_probability) {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::bernoulli_distribution bernoulli_distribution(true_probability);
  return bernoulli_distribution(generator);
}

std::vector<Graph::VertexId> get_unconnected_vertex_ids(
    const Graph& graph,
    Graph::VertexId vertex_id) {
  std::vector<Graph::VertexId> unconnected_vertex_ids = {};
  for (const auto next_depth_vertex_id :
       graph.get_depth_vertex_ids(graph.get_vertex_depth(vertex_id) + 1)) {
    if (!graph.is_vertices_connected(vertex_id, next_depth_vertex_id)) {
      unconnected_vertex_ids.push_back(next_depth_vertex_id);
    }
  }

  return unconnected_vertex_ids;
}

Graph::VertexId get_random_vertex_id(
    const std::vector<Graph::VertexId>& vertex_ids) {
  assert((!vertex_ids.empty()) &&
         "Can't pick random vertex id from empty list");

  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<> uniform_int_distribution(
      0, vertex_ids.size() - 1);

  return vertex_ids[uniform_int_distribution(generator)];
}

void generate_green_edges(Graph& graph, std::mutex& graph_mutex) {
  for (Graph::Depth current_depth = kGraphDefaultDepth;
       current_depth <= graph.get_depth(); current_depth++) {
    const auto& current_depth_vertex_ids =
        graph.get_depth_vertex_ids(current_depth);
    if (!current_depth_vertex_ids.empty()) {
      std::for_each(current_depth_vertex_ids.begin(),
                    current_depth_vertex_ids.end(),
                    [&graph, &graph_mutex](Graph::VertexId vertex_id) {
                      if (get_random_bool(kEdgeGreenProbability)) {
                        const std::lock_guard lock(graph_mutex);
                        graph.add_edge(vertex_id, vertex_id);
                      }
                    });
    }
  }
}

void generate_yellow_edges(Graph& graph, std::mutex& graph_mutex) {
  const auto graph_depth = graph.get_depth();

  for (Graph::Depth current_depth = kGraphDefaultDepth;
       current_depth <= graph_depth - kYellowEdgeLength; current_depth++) {
    float new_edge_probability = current_depth / (graph_depth - 1.f);

    const auto& current_depth_vertex_ids =
        graph.get_depth_vertex_ids(current_depth);

    if (!current_depth_vertex_ids.empty()) {
      std::for_each(
          current_depth_vertex_ids.begin(), current_depth_vertex_ids.end(),
          [&graph, &graph_mutex,
           new_edge_probability](Graph::VertexId vertex_id) {
            if (get_random_bool(new_edge_probability)) {
              const std::lock_guard lock(graph_mutex);
              const auto& to_vertex_ids =
                  get_unconnected_vertex_ids(graph, vertex_id);

              if (to_vertex_ids.empty() == false) {
                const auto to_vertex_id = get_random_vertex_id(to_vertex_ids);

                graph.add_edge(vertex_id, to_vertex_id);
              }
            }
          });
    }
  }
}

void generate_red_edges(Graph& graph, std::mutex& graph_mutex) {
  const auto max_depth = graph.get_depth() - kRedEdgeLength;
  for (Graph::Depth current_depth = kGraphDefaultDepth;
       current_depth <= max_depth; current_depth++) {
    const auto& to_vertex_ids =
        graph.get_depth_vertex_ids(current_depth + kRedEdgeLength);

    if (to_vertex_ids.empty()) {
      break;
    }

    const auto& current_depth_vertex_ids =
        graph.get_depth_vertex_ids(current_depth);
    if (!current_depth_vertex_ids.empty()) {
      std::for_each(
          current_depth_vertex_ids.begin(), current_depth_vertex_ids.end(),
          [&graph, &graph_mutex, &to_vertex_ids](Graph::VertexId vertex_id) {
            if (get_random_bool(kEdgeRedProbability)) {
              const auto to_vertex_id = get_random_vertex_id(to_vertex_ids);
              const std::lock_guard lock(graph_mutex);
              graph.add_edge(vertex_id, to_vertex_id);
            }
          });
    }
  }
}
}  // namespace

void GraphGenerator::generate_grey_branch(Graph& graph,
                                          Graph::VertexId root_vertex_id,
                                          Graph::Depth current_depth,
                                          std::mutex& graph_mutex) const {
  const float new_vertex_probability =
      1.f - (current_depth - 1.f) / (params_.depth() - 1.f);

  if (!get_random_bool(new_vertex_probability)) {
    return;
  }

  const auto new_vertex_id = [&graph_mutex, &graph, root_vertex_id]() {
    const std::lock_guard lock(graph_mutex);
    const auto new_vertex_id = graph.add_vertex();
    graph.add_edge(root_vertex_id, new_vertex_id);
    return new_vertex_id;
  }();

  for (int attempt = 0; attempt < params_.new_vertices_count(); attempt++) {
    if (current_depth < params_.depth()) {
      generate_grey_branch(graph, new_vertex_id, current_depth + 1,
                           graph_mutex);
    }
  }
}

Graph GraphGenerator::generate() const {
  auto graph = Graph();

  if (params_.depth() != 0) {
    const auto root_id = graph.add_vertex();
    generate_grey_edges(graph, root_id);

    std::mutex graph_mutex;

    auto greed_edges_thread = std::thread(
        [&graph, &graph_mutex]() { generate_green_edges(graph, graph_mutex); });

    auto yellow_edges_thread = std::thread([&graph, &graph_mutex]() {
      generate_yellow_edges(graph, graph_mutex);
    });

    auto red_edges_thread = std::thread(
        [&graph, &graph_mutex]() { generate_red_edges(graph, graph_mutex); });

    greed_edges_thread.join();
    yellow_edges_thread.join();
    red_edges_thread.join();
  }

  return graph;
}

void GraphGenerator::generate_grey_edges(Graph& graph,
                                         Graph::VertexId root_id) const {
  std::mutex jobs_mutex, graph_mutex;

  using JobCallback = std::function<void()>;
  auto jobs = std::list<JobCallback>();

  for (int i = 0; i < params_.new_vertices_count(); i++) {
    jobs.push_back([&graph, root_id, &graph_mutex, this]() {
      generate_grey_branch(graph, root_id, graph.get_vertex_depth(root_id),
                           graph_mutex);
    });
  }

  std::atomic<bool> should_terminate = false;
  std::atomic<int> jobs_count = jobs.size();

  const auto worker = [&should_terminate, &jobs_mutex, &jobs_count, &jobs]() {
    while (true) {
      if (should_terminate) {
        return;
      }

      const auto job_optional = [&jobs, &jobs_count,
                                 &jobs_mutex]() -> std::optional<JobCallback> {
        const std::lock_guard lock(jobs_mutex);

        if (!jobs.empty()) {
          auto job = jobs.back();
          jobs.pop_back();
          jobs_count = jobs.size();
          return job;
        }
        return std::nullopt;
      }();

      const std::lock_guard lock(jobs_mutex);
      if (job_optional.has_value()) {
        const auto& job = job_optional.value();
        job();
      }
    }
  };

  const auto threads_count =
      std::min(kMaxThreadsCount, params_.new_vertices_count());
  auto threads = std::vector<std::thread>();
  threads.reserve(threads_count);

  for (int i = 0; i < threads_count; i++) {
    threads.emplace_back(worker);
  }

  while (jobs_count > 0) {
  }

  should_terminate = true;
  for (auto& thread : threads) {
    thread.join();
  }
}
}  // namespace uni_course_cpp
