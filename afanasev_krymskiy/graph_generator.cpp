#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <thread>
#include <unordered_map>
#include <vector>

#include "config.hpp"
#include "graph.hpp"
#include "graph_generator.hpp"
#include "interfaces/i_graph.hpp"
#include "interfaces/i_vertex.hpp"
#include "interfaces/i_worker.hpp"

namespace {
static const int kMaxThreadsCount = std::thread::hardware_concurrency();
bool check_probability(double probability) {
  std::random_device device;
  std::mt19937 generator(device());
  std::bernoulli_distribution distribution(probability);
  return distribution(generator);
}

uni_course_cpp::VertexId get_random_vertex_id(
    const std::vector<uni_course_cpp::VertexId>& list) {
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<> distribution(0, list.size() - 1);
  return list[distribution(generator)];
}

std::vector<uni_course_cpp::VertexId> get_unconnected_vertices_ids(
    const uni_course_cpp::Graph& graph,
    uni_course_cpp::VertexId vertex_id,
    const std::vector<uni_course_cpp::VertexId>& vertices_ids) {
  std::vector<uni_course_cpp::VertexId> result;
  for (const auto vertex_id_depth_greater : vertices_ids) {
    if (!graph.has_edge(vertex_id, vertex_id_depth_greater)) {
      result.push_back(vertex_id_depth_greater);
    }
  }
  return result;
}

void generate_green_edges(uni_course_cpp::Graph& graph,
                          std::mutex& mutex_for_graph) {
  std::for_each(
      graph.vertices().cbegin(), graph.vertices().cend(),
      [&graph, &mutex_for_graph](const auto& vertices_item) {
        if (check_probability(uni_course_cpp::config::kGreenEdgesProbability)) {
          const std::lock_guard lock(mutex_for_graph);
          graph.add_edge(vertices_item.first, vertices_item.first);
        }
      });
}

void generate_yellow_edges(uni_course_cpp::Graph& graph,
                           std::mutex& mutex_for_graph) {
  for (auto depth = uni_course_cpp::kDefaultDepth; depth < graph.depth();
       ++depth) {
    const float probability = (depth - 1) / (graph.depth() - 2.f);
    std::for_each(
        graph.vertices_at_depth(depth).cbegin(),
        graph.vertices_at_depth(depth).cend(),
        [&graph, &mutex_for_graph, probability, depth](const auto vertex_id) {
          if (check_probability(probability)) {
            const auto unconnected_vertices_ids = get_unconnected_vertices_ids(
                graph, vertex_id,
                graph.vertices_at_depth(depth +
                                        uni_course_cpp::kYellowEdgeDepth));
            if (!unconnected_vertices_ids.empty()) {
              const std::lock_guard lock(mutex_for_graph);
              graph.add_edge(vertex_id,
                             get_random_vertex_id(unconnected_vertices_ids));
            }
          }
        });
  }
}

void generate_red_edges(uni_course_cpp::Graph& graph,
                        std::mutex& mutex_for_graph) {
  for (auto depth = uni_course_cpp::kDefaultDepth; depth < graph.depth() - 1;
       ++depth) {
    const std::lock_guard lock(mutex_for_graph);
    std::for_each(
        graph.vertices_at_depth(depth).cbegin(),
        graph.vertices_at_depth(depth).cend(),
        [&graph, &mutex_for_graph, depth](const auto vertex_id) {
          if (check_probability(uni_course_cpp::config::kRedEdgesProbability) &&
              !graph.vertices_at_depth(depth + uni_course_cpp::kRedEdgeDepth)
                   .empty()) {
            graph.add_edge(vertex_id,
                           get_random_vertex_id(graph.vertices_at_depth(
                               depth + uni_course_cpp::kRedEdgeDepth)));
          }
        });
  }
}
}  // namespace

namespace uni_course_cpp {
std::unique_ptr<IGraph> GraphGenerator::generate() const {
  auto graph = Graph();
  if (params_.get_depth() == 0)
    return std::make_unique<Graph>(std::move(graph));
  generate_grey_edges(graph, graph.add_vertex());

  std::mutex mutex_for_graph;
  std::thread green_thread(generate_green_edges, std::ref(graph),
                           std::ref(mutex_for_graph));
  std::thread yellow_thread(generate_yellow_edges, std::ref(graph),
                            std::ref(mutex_for_graph));
  std::thread red_thread(generate_red_edges, std::ref(graph),
                         std::ref(mutex_for_graph));

  green_thread.join();
  yellow_thread.join();
  red_thread.join();
  return std::make_unique<Graph>(std::move(graph));
}

void GraphGenerator::generate_grey_branch(
    Graph& graph,
    std::mutex& mutex_for_graph,
    VertexId parent_vertex_id,
    GraphDepth parent_vertex_depth) const {
  if (parent_vertex_depth >= params_.get_depth())
    return;
  const float probability =
      (params_.get_depth() - parent_vertex_depth) / (params_.get_depth() - 1.f);
  for (int i = 0; i < params_.new_vertices_count(); ++i) {
    if (check_probability(probability)) {
      const auto child_vertex_id = [&graph, &mutex_for_graph,
                                    parent_vertex_id]() {
        const std::lock_guard lock(mutex_for_graph);
        const auto child_vertex_id = graph.add_vertex();
        graph.add_edge(parent_vertex_id, child_vertex_id);
        return child_vertex_id;
      }();
      generate_grey_branch(graph, mutex_for_graph, child_vertex_id,
                           parent_vertex_depth + 1);
    }
  }
}

void GraphGenerator::generate_grey_edges(Graph& graph,
                                         VertexId root_vertex_id) const {
  if (params_.get_depth() <= kDefaultDepth)
    return;

  using JobCallback = std::function<void()>;
  auto jobs = std::list<JobCallback>();
  std::mutex mutex_for_graph;
  std::mutex mutex_for_jobs;
  std::atomic<int> number_of_jobs = params_.new_vertices_count();

  for (int i = 0; i < number_of_jobs; ++i) {
    jobs.push_back([&graph, &mutex_for_graph, root_vertex_id, this]() {
      generate_grey_branch(graph, mutex_for_graph, root_vertex_id,
                           kDefaultDepth);
    });
  }

  std::atomic<bool> should_terminate = false;
  const auto worker = [&should_terminate, &mutex_for_jobs, &jobs,
                       &number_of_jobs]() {
    while (true) {
      if (should_terminate) {
        return;
      }
      const auto job_optional =
          [&jobs, &mutex_for_jobs]() -> std::optional<JobCallback> {
        const std::lock_guard lock(mutex_for_jobs);
        if (!jobs.empty()) {
          const auto item = jobs.back();
          jobs.pop_back();
          return item;
        }
        return std::nullopt;
      }();
      if (job_optional.has_value()) {
        --number_of_jobs;
        const auto& job = job_optional.value();
        job();
      }
    }
  };

  const auto threads_count =
      std::min(kMaxThreadsCount, params_.new_vertices_count());
  auto threads = std::vector<std::thread>();
  threads.reserve(threads_count);

  for (int i = 0; i < threads_count; ++i) {
    threads.emplace_back(worker);
  }

  while (number_of_jobs) {
  }

  should_terminate = true;
  for (auto& thread : threads) {
    thread.join();
  }
}
}  // namespace uni_course_cpp
