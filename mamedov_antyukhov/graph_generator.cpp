#include "graph_generator.hpp"
#include <atomic>
#include <cassert>
#include <functional>
#include <optional>
#include <queue>
#include <random>
#include <thread>

namespace uni_course_cpp {

namespace {
static constexpr double kGreenEdgesProbability = 0.1;
static constexpr double kRedEdgesProbability = 0.33;
static constexpr GraphDepth kGraphBaseDepth = 1;
static constexpr GraphDepth kYellowEdgeDepth = 1;
static constexpr GraphDepth kRedEdgeDepth = 2;
static constexpr GraphDepth kYellowMaxEdgeDepth = 2;
static const int kMaxThreadsCount = std::thread::hardware_concurrency();

bool check_probability(double probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution d(probability);
  return d(gen);
}

VertexId get_random_vertex_id(int size) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, size);
  return distrib(gen);
}

std::vector<VertexId> get_unconnected_vertex_ids(
    const std::vector<VertexId>& vertices,
    const Graph& graph,
    VertexId vertex) {
  std::vector<VertexId> result;
  for (const auto cur_vertex : vertices) {
    if (!graph.has_edge(vertex, cur_vertex)) {
      result.emplace_back(cur_vertex);
    }
  }
  return result;
}

void generate_green_edges(Graph& graph, std::mutex& graph_mutex) {
  const auto& vertices = graph.get_vertices();
  std::for_each(vertices.begin(), vertices.end(),
                [&graph, &graph_mutex](const auto& vertex) {
                  if (check_probability(kGreenEdgesProbability)) {
                    const std::lock_guard<std::mutex> graph_lock(graph_mutex);
                    graph.add_edge(vertex.first, vertex.first);
                  }
                });
}

void generate_yellow_edges(Graph& graph, std::mutex& graph_mutex) {
  const auto max_depth = graph.depth() - kYellowMaxEdgeDepth;
  for (GraphDepth current_depth = kGraphBaseDepth; current_depth <= max_depth;
       ++current_depth) {
    const auto& currrent_level_vertex_ids =
        graph.get_vertex_ids_at_depth(current_depth);
    std::for_each(
        currrent_level_vertex_ids.begin(), currrent_level_vertex_ids.end(),
        [&graph, &graph_mutex, max_depth, current_depth](VertexId vertex_id) {
          if (check_probability((1.0 / max_depth) * current_depth)) {
            const auto next_level_vertex_ids = get_unconnected_vertex_ids(
                graph.get_vertex_ids_at_depth(current_depth + kYellowEdgeDepth),
                graph, vertex_id);
            if (!next_level_vertex_ids.empty()) {
              const std::lock_guard<std::mutex> graph_lock(graph_mutex);
              graph.add_edge(
                  vertex_id,
                  next_level_vertex_ids[get_random_vertex_id(
                      next_level_vertex_ids.size() - kYellowEdgeDepth)]);
            }
          }
        });
  }
}

void generate_red_edges(Graph& graph, std::mutex& graph_mutex) {
  const auto max_depth = graph.depth() - kRedEdgeDepth;
  for (GraphDepth current_depth = 0; current_depth < max_depth;
       ++current_depth) {
    const auto& current_level_vertex_ids =
        graph.get_vertex_ids_at_depth(current_depth);
    std::for_each(
        current_level_vertex_ids.begin(), current_level_vertex_ids.end(),
        [&graph, &graph_mutex, current_depth](VertexId vertex_id) {
          if (check_probability(kRedEdgesProbability)) {
            const auto next_level_vertex_ids =
                graph.get_vertex_ids_at_depth(current_depth + kRedEdgeDepth);
            const std::lock_guard<std::mutex> graph_lock(graph_mutex);
            graph.add_edge(
                vertex_id,
                next_level_vertex_ids[get_random_vertex_id(
                    next_level_vertex_ids.size() - kGraphBaseDepth)]);
          }
        });
  }
}

};  // namespace

std::unique_ptr<IGraph> GraphGenerator::generate() const {
  std::mutex graph_mutex;
  auto graph = Graph();
  if (!params_.depth()) {
    return std::make_unique<Graph>(std::move(graph));
  }
  const auto root_vertex_id = graph.add_vertex();
  if (params_.depth() == kGraphBaseDepth) {
    return std::make_unique<Graph>(std::move(graph));
  }

  generate_grey_edges(graph, graph_mutex, root_vertex_id);
  std::thread generating_green_edges(
      [&graph, &graph_mutex]() { generate_green_edges(graph, graph_mutex); });
  std::thread generating_yellow_edges(
      [&graph, &graph_mutex]() { generate_yellow_edges(graph, graph_mutex); });
  std::thread generating_red_edges(
      [&graph, &graph_mutex]() { generate_red_edges(graph, graph_mutex); });
  generating_green_edges.join();
  generating_yellow_edges.join();
  generating_red_edges.join();
  return std::make_unique<Graph>(std::move(graph));
}

void GraphGenerator::generate_grey_branch(Graph& graph,
                                          std::mutex& graph_mutex,
                                          VertexId root_vertex_id,
                                          GraphDepth current_depth) const {
  const double probability =
      1.0 - (1.0 / (params_.depth() - kGraphBaseDepth)) * current_depth;

  if (current_depth == params_.depth()) {
    return;
  }

  const VertexId new_vertex_id = [&graph, &graph_mutex, root_vertex_id]() {
    const std::lock_guard graph_lock(graph_mutex);
    const auto new_vertex_id = graph.add_vertex();
    graph.add_edge(root_vertex_id, new_vertex_id);
    return new_vertex_id;
  }();

  for (int i = 0; i < params_.new_vertices_count(); ++i) {
    if (check_probability(probability)) {
      generate_grey_branch(graph, graph_mutex, new_vertex_id,
                           current_depth + 1);
    }
  }
}

void GraphGenerator::generate_grey_edges(Graph& graph,
                                         std::mutex& graph_mutex,
                                         VertexId root_vertex_id) const {
  using JobCallback = std::function<void()>;
  auto jobs = std::queue<JobCallback>();
  const auto new_vertices_count = params_.new_vertices_count();
  std::atomic<int> jobs_number = new_vertices_count;
  std::atomic<bool> should_terminate = false;
  std::mutex jobs_mutex;

  for (int i = 0; i < new_vertices_count; ++i) {
    jobs.push([&graph, &graph_mutex, root_vertex_id, this]() {
      generate_grey_branch(graph, graph_mutex, root_vertex_id, kGraphBaseDepth);
    });
  }

  const auto worker = [&should_terminate, &jobs_number, &jobs_mutex, &jobs]() {
    while (!should_terminate) {
      const auto job_optional = [&jobs_mutex,
                                 &jobs]() -> std::optional<JobCallback> {
        const std::lock_guard<std::mutex> job_lock(jobs_mutex);
        if (!jobs.empty()) {
          auto job = jobs.front();
          jobs.pop();
          return job;
        }
        return std::nullopt;
      }();

      if (job_optional.has_value()) {
        const auto& job = job_optional.value();
        job();
        --jobs_number;
      }
    }
  };

  const auto threads_count = std::min(kMaxThreadsCount, new_vertices_count);
  std::vector<std::thread> threads;
  threads.reserve(threads_count);

  for (int i = 0; i < threads_count; i++) {
    threads.emplace_back(worker);
  }

  while (jobs_number) {
  }
  should_terminate = true;

  for (auto& thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

}  // namespace uni_course_cpp
