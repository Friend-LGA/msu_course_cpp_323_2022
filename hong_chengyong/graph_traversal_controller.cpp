#include "graph_traversal_controller.hpp"
#include <algorithm>
#include <cassert>
#include <mutex>
#include <thread>
#include "graph.hpp"
#include "graph_path.hpp"
#include "graph_traverser.hpp"

namespace {

const int MAX_WORKERS_COUNT = std::thread::hardware_concurrency();
}  // namespace

namespace uni_course_cpp {

GraphTraversalController::GraphTraversalController(
    const std::vector<Graph>& graphs)
    : graphs_(graphs) {
  threads_num_ = std::min(MAX_WORKERS_COUNT, static_cast<int>(graphs.size()));
  for (int i = 0; i < threads_num_; i++) {
    workers_.emplace_back([&jobs_ = jobs_, &mutex_jobs_ = mutex_jobs_,
                           this]() -> std::optional<std::function<void()>> {
      const std::lock_guard lock(mutex_jobs_);
      if (!jobs_.empty()) {
        const auto job = jobs_.front();
        jobs_.pop_front();
        return job;
      }
      return std::nullopt;
    });
  }
}

void GraphTraversalController::Worker::start() {
  assert(state_ == State::Idle && "Worker is already in process");

  state_ = State::Working;
  thread_ = std::thread(
      [&state_ = state_, &get_job_callback_ = get_job_callback_, this]() {
        while (true) {
          if (state_ == State::ShouldTerminate) {
            state_ = State::Idle;
            return;
          }
          const auto job_optional = get_job_callback_();
          if (job_optional.has_value()) {
            const auto& job = job_optional.value();
            job();
          }
        }
      });
}

void GraphTraversalController::Worker::stop() {
  assert(state_ == State::Working && "Worker has already stopped");
  state_ = State::ShouldTerminate;
  thread_.join();
}

GraphTraversalController::Worker::~Worker() {
  if (state_ == State::Working) {
    stop();
  }
}

void GraphTraversalController::traverse(
    const TraversalStartedCallback& traversalStartedCallback,
    const TraversalFinishedCallback& traversalFinishedCallback) {
  for (int i = 0; i < graphs_.size(); i++) {
    jobs_.emplace_back([&traversalStartedCallback, &traversalFinishedCallback,
                        &graphs_traversed_ = graphs_traversed_,
                        &mutex_start_ = mutex_start_,
                        &mutex_finish_ = mutex_finish_, i, this]() {
      {
        const std::lock_guard lock(mutex_start_);
        traversalStartedCallback(i);
      }
      GraphTraverser graph_traverser(graphs_[i]);
      const auto paths = graph_traverser.findAllPaths();
      {
        const std::lock_guard lock(mutex_finish_);
        traversalFinishedCallback(i, std::move(paths));
      }
      graphs_traversed_++;
    });
  }

  for (auto& worker : workers_) {
    worker.start();
  }
  while (graphs_traversed_ != graphs_.size()) {
  }
  for (auto& worker : workers_) {
    worker.stop();
  }
}
}  // namespace uni_course_cpp
