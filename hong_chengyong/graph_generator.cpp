#include "graph_generator.hpp"
#include <atomic>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <optional>
#include <random>
#include <thread>
#include <vector>
#include "graph.hpp"

namespace {
constexpr float GREEN_PROBABILITY = 0.1, RED_PROBABILITY = 0.33;
float getGreyProbability(float step, uni_course_cpp::Depth depth) {
  return 1.0 - step * depth;
}
float getYellowProbability(const uni_course_cpp::Graph& graph,
                           const uni_course_cpp::VertexId& vertex_id) {
  return 1.0 * graph.vertexDepth(vertex_id) / (graph.depth() - 1);
}
bool randomValue(float probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution distribution(probability);
  return distribution(gen);
}
int getRandomVertexId(const std::vector<uni_course_cpp::VertexId>& vertex_ids) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> random_number(0, vertex_ids.size() - 1);
  return vertex_ids.at(random_number(gen));
}
void generateGreenEdges(uni_course_cpp::Graph& graph, std::mutex& mutex) {
  for (const auto& vertex : graph.vertexes()) {
    if (randomValue(GREEN_PROBABILITY)) {
      const std::lock_guard lock(mutex);
      graph.addEdge(vertex.id, vertex.id);
    }
  }
}
void generateYellowEdges(uni_course_cpp::Graph& graph, std::mutex& mutex) {
  for (const auto& vertex : graph.vertexes()) {
    int vertex_depth = graph.vertexDepth(vertex.id);
    if (vertex_depth < graph.depth() &&
        randomValue(getYellowProbability(graph, vertex.id))) {
      std::vector<uni_course_cpp::VertexId> next_layer;
      for (const auto& vertex_id : graph.vertexIdsAtLayer(vertex_depth + 1)) {
        if (!graph.areConnected(vertex.id, vertex_id)) {
          next_layer.push_back(vertex_id);
        }
      }
      const std::lock_guard lock(mutex);
      graph.addEdge(vertex.id, getRandomVertexId(next_layer));
    }
  }
}
void generateRedEdges(uni_course_cpp::Graph& graph, std::mutex& mutex) {
  for (const auto& vertex : graph.vertexes()) {
    int vertex_depth = graph.vertexDepth(vertex.id);
    if (randomValue(RED_PROBABILITY) && vertex_depth < (graph.depth() - 1)) {
      const std::lock_guard lock(mutex);
      graph.addEdge(vertex.id, getRandomVertexId(
                                   graph.vertexIdsAtLayer(vertex_depth + 2)));
    }
  }
}
}  // namespace

namespace uni_course_cpp {

void GraphGenerator::generateGrayBranch(Graph& graph,
                                        int branch_depth,
                                        const VertexId& root_id,
                                        std::mutex& mutex) const {
  if (branch_depth == params_.depth) {
    return;
  }
  for (int i = 0; i < params_.new_vertexes_num; i++) {
    if (randomValue(getGreyProbability(1.0 / params_.depth, branch_depth))) {
      const VertexId new_vertex_id = [&graph, &mutex, &root_id]() {
        const std::lock_guard lock(mutex);
        const auto new_vertex_id = graph.addVertex();
        graph.addEdge(root_id, new_vertex_id);
        return new_vertex_id;
      }();
      generateGrayBranch(graph, branch_depth + 1, new_vertex_id, mutex);
    }
  }
}

Graph GraphGenerator::generateMainBody() const {
  Graph graph;
  VertexId root_id = graph.addVertex();
  using JobCallback = std::function<void()>;
  auto jobs = std::list<JobCallback>();  //
  std::mutex mutex;
  std::atomic<int> created_branches = 0;
  bool should_terminate = false;
  for (int i = 0; i < params_.new_vertexes_num; i++) {
    jobs.push_back([&graph, &mutex, &root_id, &created_branches, this]() {
      generateGrayBranch(graph, 0, root_id, mutex);
      created_branches++;
    });
  }
  auto worker = [&should_terminate, &mutex, &jobs]() {
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

  const int MAX_THREADS_COUNT =
      std::thread::hardware_concurrency();  // 获取硬件最大支持线程
  const auto threads_count =
      std::min(MAX_THREADS_COUNT, params_.new_vertexes_num);
  auto threads = std::vector<std::thread>();
  threads.reserve(threads_count);

  for (int i = 0; i < threads_count; i++) {
    threads.push_back(std::thread(worker));
  }

  while (created_branches < params_.new_vertexes_num) {
  }

  should_terminate = true;
  for (auto& thread : threads) {
    thread.join();
  }
  if (graph.depth() < params_.depth) {
    std::cout << "Depth of the graph is less than given. Depth is "
              << graph.depth() << std::endl;
  }
  return graph;
}

Graph GraphGenerator::generate() const {
  Graph graph = generateMainBody();
  std::mutex mutex;
  std::thread green_thread(
      [&graph, &mutex, this]() { generateGreenEdges(graph, mutex); });
  std::thread yellow_thread(
      [&graph, &mutex, this]() { generateYellowEdges(graph, mutex); });
  std::thread red_thread(
      [&graph, &mutex, this]() { generateRedEdges(graph, mutex); });
  green_thread.join();
  yellow_thread.join();
  red_thread.join();
  return graph;
}
}  // namespace uni_course_cpp
