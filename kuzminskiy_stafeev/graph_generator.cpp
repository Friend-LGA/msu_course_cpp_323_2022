#include "graph_generator.hpp"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <optional>
#include <random>
#include <thread>

namespace uni_course_cpp {

namespace {

const int kMaxThreadsCount = std::thread::hardware_concurrency();
constexpr GraphDepth kGraphBaseDepth = 1;
constexpr GraphDepth kYellowEdgesDiff = 1;
constexpr GraphDepth kRedEdgesDiff = 2;

using JobCallBack = std::function<void()>;

std::vector<VertexId> get_unconnected_vertex_ids(
    const Graph& graph,
    VertexId vertex_id,
    const std::vector<VertexId>& vertex_ids_on_depth) {
  std::vector<VertexId> unconnected_vertices = {};
  for (const auto cur_vertex_id : vertex_ids_on_depth) {
    if (!graph.is_connected(vertex_id, cur_vertex_id)) {
      unconnected_vertices.emplace_back(cur_vertex_id);
    }
  }

  return unconnected_vertices;
}

int get_random_vertex(const std::vector<VertexId>& vertex_ids) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, vertex_ids.size() - 1);
  return vertex_ids[dis(gen)];
}

bool check_probability(float prob) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution d(prob);
  return d(gen);
}

void generate_green_edges(Graph& graph, std::mutex& graph_mutex) {
  static float constexpr prob = 0.1;
  const std::unordered_map<VertexId, std::unique_ptr<IVertex>>& vertices =
      graph.vertices();

  std::for_each(
      vertices.begin(), vertices.end(),
      [&graph, &graph_mutex](
          const std::pair<const VertexId, std::unique_ptr<IVertex>>& element) {
        const VertexId vertex_id = element.first;
        if (check_probability(prob)) {
          const std::lock_guard<std::mutex> guard(graph_mutex);
          graph.add_edge(vertex_id, vertex_id);
        }
      });
}

void generate_yellow_edges(Graph& graph,
                           GraphDepth depth,
                           std::mutex& graph_mutex) {
  const float step = 1.0 / (depth - 2);
  for (GraphDepth cur_depth = kGraphBaseDepth;
       cur_depth <= depth - kYellowEdgesDiff; cur_depth++) {
    const float prob = (float)(step * (cur_depth - 1));
    const auto& vertices = graph.vertices_of_depth(cur_depth);

    std::for_each(
        vertices.begin(), vertices.end(),
        [&graph, &graph_mutex, cur_depth, prob](const VertexId from_vertex_id) {
          if (check_probability(prob)) {
            const auto unconnected_vertices_ids = get_unconnected_vertex_ids(
                graph, from_vertex_id,
                graph.vertices_of_depth(cur_depth + kYellowEdgesDiff));
            if (!unconnected_vertices_ids.empty()) {
              const auto to_vertex_id =
                  get_random_vertex(unconnected_vertices_ids);
              const std::lock_guard<std::mutex> guard(graph_mutex);
              graph.add_edge(from_vertex_id, to_vertex_id);
            }
          }
        });
  }
}

void generate_red_edges(Graph& graph,
                        GraphDepth depth,
                        std::mutex& graph_mutex) {
  static float constexpr prob = (float)1 / 3;
  for (GraphDepth cur_depth = kGraphBaseDepth;
       cur_depth <= depth - kRedEdgesDiff; cur_depth++) {
    const auto& depth_vertices = graph.vertices_of_depth(cur_depth);
    const auto& deeper_vertices =
        graph.vertices_of_depth(cur_depth + kRedEdgesDiff);

    std::for_each(depth_vertices.begin(), depth_vertices.end(),
                  [&graph, &graph_mutex,
                   &deeper_vertices](const VertexId from_vertex_id) {
                    const auto to_vertex_id =
                        get_random_vertex(deeper_vertices);
                    if (check_probability(prob)) {
                      const std::lock_guard<std::mutex> guard(graph_mutex);
                      graph.add_edge(from_vertex_id, to_vertex_id);
                    }
                  });
  }
}

}  // namespace

std::unique_ptr<IGraph> GraphGenerator::generate() const {
  std::mutex graph_mutex;
  auto graph = Graph();

  if (params_.depth()) {
    const auto root_id = graph.add_vertex();
    generate_grey_edges(graph, root_id, graph_mutex);
    std::thread green_thread(generate_green_edges, std::ref(graph),
                             std::ref(graph_mutex));
    std::thread yellow_thread(&generate_yellow_edges, std::ref(graph),
                              graph.get_graph_depth(), std::ref(graph_mutex));
    std::thread red_thread(&generate_red_edges, std::ref(graph),
                           graph.get_graph_depth(), std::ref(graph_mutex));

    green_thread.join();
    yellow_thread.join();
    red_thread.join();
  }

  return std::make_unique<Graph>(std::move(graph));
}

void GraphGenerator::generate_grey_branch(Graph& graph,
                                          std::mutex& graph_mutex,
                                          VertexId root_id,
                                          GraphDepth cur_depth) const {
  const auto depth = params_.depth();

  if (depth < cur_depth) {
    return;
  }

  const float step = 1.0 / (depth - kGraphBaseDepth);
  const float prob = (float)(1 - step * cur_depth);

  const VertexId child_id = [&graph, &graph_mutex, root_id]() {
    const std::lock_guard<std::mutex> guard(graph_mutex);
    const VertexId child_id = graph.add_vertex();
    graph.add_edge(root_id, child_id);
    return child_id;
  }();

  for (VertexId cur_vertex_count = 0;
       cur_vertex_count < params_.new_vertices_count(); cur_vertex_count++) {
    if (check_probability(prob)) {
      generate_grey_branch(graph, graph_mutex, child_id, cur_depth + 1);
    }
  }
}

void GraphGenerator::generate_grey_edges(Graph& graph,
                                         VertexId root_id,
                                         std::mutex& graph_mutex) const {
  const auto depth = params_.depth();
  if (depth == kGraphBaseDepth) {
    return;
  }

  auto jobs = std::list<JobCallBack>();
  const auto new_vertices_count = params_.new_vertices_count();

  std::atomic<int> jobs_count(new_vertices_count);

  for (int i = 0; i < new_vertices_count; i++) {
    jobs.push_back([&graph, &graph_mutex, &root_id, this]() {
      if (params_.depth() > kGraphBaseDepth) {
        generate_grey_branch(graph, graph_mutex, root_id, kGraphBaseDepth);
      }
    });
  }

  std::atomic<bool> should_terminate(false);
  std::mutex jobs_mutex;
  const auto worker = [&should_terminate, &jobs_count, &jobs_mutex, &jobs]() {
    while (true) {
      if (should_terminate) {
        return;
      }

      const auto job_optional = [&jobs_mutex, &should_terminate,
                                 &jobs]() -> std::optional<JobCallBack> {
        const std::lock_guard<std::mutex> guard(jobs_mutex);

        if (!jobs.empty()) {
          auto job = jobs.front();
          jobs.pop_front();
          return job;
        }

        return std::nullopt;
      }();

      if (job_optional.has_value()) {
        const auto& job = job_optional.value();
        job();
        jobs_count -= 1;
      }
    }
  };

  const auto threads_count = std::min(kMaxThreadsCount, new_vertices_count);
  std::vector<std::thread> threads;
  threads.reserve(threads_count);

  for (int i = 0; i < threads_count; i++) {
    threads.emplace_back(worker);
  }

  while (jobs_count != 0) {
  }

  should_terminate = true;

  for (auto& thread : threads) {
    thread.join();
  }
}

}  // namespace uni_course_cpp
