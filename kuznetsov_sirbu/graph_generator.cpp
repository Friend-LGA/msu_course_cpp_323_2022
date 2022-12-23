#include "graph_generator.hpp"
#include <atomic>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <random>
#include <thread>
#include <utility>

namespace uni_course_cpp {

namespace {

static constexpr double kProbabilityRed = 0.33;
static constexpr double kProbabilityGreen = 0.1;
static const int kMaxThreadsCount = std::thread::hardware_concurrency();

bool check_probability(double chance) {
  std::mt19937 generator{std::random_device()()};
  std::bernoulli_distribution distribution(chance);
  return distribution(generator);
}

double probaility_generate_grey_edge(GraphDepth current_depth,
                                     GraphDepth graph_depth) {
  if (graph_depth == 0) {
    return 1.0;
  } else {
    return 1.0 - static_cast<double>((current_depth - Graph::kBaseDepth)) /
                     (graph_depth - Graph::kBaseDepth);
  }
}

VertexId get_random_vertex_id(int size) {
  std::random_device rand_device;
  std::mt19937 gen(rand_device());
  std::uniform_int_distribution<> distrib(0, size - 1);
  return distrib(gen);
}

std::vector<VertexId> get_unconnected_vertex_ids(
    const Graph& graph,
    VertexId vertex_from_id,
    const std::vector<VertexId>& vertex_ids,
    std::mutex& graph_mutex) {
  std::vector<VertexId> not_connected_vertex_ids;
  for (const auto& vertex_to_id : vertex_ids) {
    const auto is_connected = [&graph, vertex_from_id, vertex_to_id,
                               &graph_mutex]() {
      const std::lock_guard<std::mutex> graph_lock(graph_mutex);
      return graph.is_connected(vertex_from_id, vertex_to_id);
    }();
    if (!is_connected) {
      not_connected_vertex_ids.emplace_back(vertex_to_id);
    }
  }
  return not_connected_vertex_ids;
}

void add_red_edge(Graph& graph,
                  VertexId vertex_from_id,
                  VertexId vertex_to_id,
                  std::mutex& graph_mutex) {
  const GraphDepth vertex_from_depth = graph.vertex_depth(vertex_from_id);
  const std::lock_guard<std::mutex> graph_lock(graph_mutex);
  graph.add_edge(vertex_from_id, vertex_to_id);
}

void generate_red_edges(Graph& graph, std::mutex& graph_mutex) {
  const auto& vertices = graph.get_vertices();
  std::for_each(
      vertices.begin(), vertices.end(),
      [&graph, &graph_mutex](
          const std::unique_ptr<uni_course_cpp::IVertex>& vertex_from) {
        if (!check_probability(kProbabilityRed)) {
          const auto vertex_from_id = vertex_from->id();
          const GraphDepth vertex_depth = graph.vertex_depth(vertex_from_id);
          const auto& vertex_ids = graph.get_vertices_with_depth(
              vertex_depth + Graph::kDifferenceRedEdge);
          if (!vertex_ids.empty()) {
            const VertexId vertex_to_id =
                vertex_ids.at(get_random_vertex_id(vertex_ids.size()));

            add_red_edge(graph, vertex_from_id, vertex_to_id, graph_mutex);
          }
        }
      });
}

void try_generate_green_edge(Graph& graph,
                             VertexId vertex_id,
                             std::mutex& graph_mutex) {
  if (check_probability(kProbabilityGreen)) {
    const std::lock_guard<std::mutex> graph_lock(graph_mutex);
    graph.add_edge(vertex_id, vertex_id);
  }
}

void generate_green_edges(Graph& graph, std::mutex& graph_mutex) {
  const auto& vertices = graph.get_vertices();
  std::for_each(vertices.begin(), vertices.end(),
                [&graph, &graph_mutex](
                    const std::unique_ptr<uni_course_cpp::IVertex>& vertex) {
                  try_generate_green_edge(graph, vertex->id(), graph_mutex);
                });
}
void add_yellow_edge(Graph& graph,
                     VertexId vertex_from_id,
                     VertexId vertex_to_id,
                     std::mutex& graph_mutex) {
  const GraphDepth vertex_from_depth = graph.vertex_depth(vertex_from_id);
  const std::lock_guard<std::mutex> graph_lock(graph_mutex);
  graph.add_edge(vertex_from_id, vertex_to_id);
}

void generate_yellow_edges(Graph& graph, std::mutex& graph_mutex) {
  const auto& vertices = graph.get_vertices();
  std::for_each(
      vertices.begin(), vertices.end(),
      [&graph, &graph_mutex](
          const std::unique_ptr<uni_course_cpp::IVertex>& vertex_from) {
        const auto vertex_from_id = vertex_from->id();
        const GraphDepth vertex_depth = graph.vertex_depth(vertex_from_id);
        const double probability_generate =
            static_cast<double>((vertex_depth - Graph::kBaseDepth)) /
            (graph.depth() - Graph::kBaseDepth - Graph::kDifferenceYellowEdge);

        if (check_probability(probability_generate)) {
          const auto& vertex_ids = graph.get_vertices_with_depth(
              vertex_depth + Graph::kDifferenceYellowEdge);
          const auto not_connected_vertex_ids = get_unconnected_vertex_ids(
              graph, vertex_from_id, vertex_ids, graph_mutex);
          if (!not_connected_vertex_ids.empty()) {
            const VertexId vertex_to_id = not_connected_vertex_ids.at(
                get_random_vertex_id(not_connected_vertex_ids.size()));
            add_yellow_edge(graph, vertex_from_id, vertex_to_id, graph_mutex);
          }
        }
      });
}

}  // namespace

std::unique_ptr<IGraph> GraphGenerator::generate() const {
  auto graph = Graph();
  std::mutex graph_mutex;

  if (params_.depth() < 0) {
    throw std::runtime_error("Graph depth < 0");
  }
  if (params_.depth() > 0) {
    const auto root_id = graph.add_vertex();

    generate_grey_edges(graph, graph_mutex, root_id);
    std::thread generating_green_edges(
        [&graph, &graph_mutex]() { generate_green_edges(graph, graph_mutex); });
    std::thread generating_yellow_edges([&graph, &graph_mutex]() {
      generate_yellow_edges(graph, graph_mutex);
    });
    std::thread generating_red_edges(
        [&graph, &graph_mutex]() { generate_red_edges(graph, graph_mutex); });
    generating_green_edges.join();
    generating_yellow_edges.join();
    generating_red_edges.join();
  }
  return std::make_unique<Graph>(std::move(graph));
}

VertexId GraphGenerator::add_grey_edge(Graph& graph,
                                       GraphDepth current_depth,
                                       VertexId vertex_id,
                                       std::mutex& graph_mutex) const {
  const std::lock_guard graph_lock(graph_mutex);
  const VertexId next_vertex_id = graph.add_vertex();
  graph.add_edge(vertex_id, next_vertex_id);
  return next_vertex_id;
}

void GraphGenerator::generate_grey_branch(Graph& graph,
                                          std::mutex& graph_mutex,
                                          VertexId root_vertex_id,
                                          GraphDepth current_depth) const {
  if (current_depth == params_.depth()) {
    return;
  }
  const auto probability =
      probaility_generate_grey_edge(current_depth, params_.depth());
  if (check_probability(probability)) {
    const VertexId next_vertex_id =
        add_grey_edge(graph, current_depth, root_vertex_id, graph_mutex);
    for (int i = 0; i < params_.new_vertices_count(); ++i) {
      generate_grey_branch(graph, graph_mutex, next_vertex_id,
                           current_depth + 1);
    }
  }
}

void GraphGenerator::generate_grey_edges(Graph& graph,
                                         std::mutex& graph_mutex,
                                         VertexId root_vertex_id) const {
  using JobCallback = std::function<void()>;
  auto jobs = std::deque<JobCallback>();
  const auto new_vertices_count = params_.new_vertices_count();
  std::atomic<bool> should_terminate = false;
  std::atomic<int> jobs_number = new_vertices_count;
  std::mutex jobs_mutex;

  for (int i = 0; i < new_vertices_count; ++i) {
    jobs.push_back([&graph, &graph_mutex, root_vertex_id, this]() {
      generate_grey_branch(graph, graph_mutex, root_vertex_id,
                           Graph::kBaseDepth);
    });
  }

  const auto worker = [&should_terminate, &jobs_number, &jobs_mutex, &jobs]() {
    while (true) {
      if (should_terminate) {
        return;
      }
      const auto job_optional = [&jobs_mutex,
                                 &jobs]() -> std::optional<JobCallback> {
        const std::lock_guard<std::mutex> job_lock(jobs_mutex);
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
    thread.join();
  }
}

}  // namespace uni_course_cpp
