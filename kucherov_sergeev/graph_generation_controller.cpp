#include <atomic>
#include <cassert>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

#include "graph_generation_controller.hpp"

namespace uni_course_cpp {
void GraphGenerationController::Worker::start() {
  assert(state_ == State::Idle);

  state_ = State::Working;

  thread_ =
      std::thread([&state = state_, &get_job_callback = get_job_callback_]() {
        while (true) {
          if (state == State::ShouldTerminate) {
            return;
          }

          const auto job_optional = get_job_callback();
          if (job_optional.has_value()) {
            const auto& job = job_optional.value();
            job();
          }
        }
      });
}

void GraphGenerationController::Worker::stop() {
  assert(state_ == State::Working);

  state_ = State::ShouldTerminate;
  thread_.join();
  state_ = State::Idle;
}

GraphGenerationController::Worker::~Worker() {
  if (state_ == State::Working) {
    stop();
  }
}

GraphGenerationController::GraphGenerationController(
    int threads_count,
    int graphs_count,
    GraphGenerator::Params&& graph_generator_params)
    : threads_count_(threads_count),
      graphs_count_(graphs_count),
      graph_generator_(std::move(graph_generator_params)) {
  const auto job_optional = [&jobs = jobs_,
                             &jobs_mutex =
                                 jobs_mutex_]() -> std::optional<JobCallback> {
    const std::lock_guard lock(jobs_mutex);

    if (!jobs.empty()) {
      auto job = jobs.back();
      jobs.pop_back();

      return job;
    }
    return std::nullopt;
  };

  for (int i = 0; i < threads_count_; i++) {
    workers_.emplace_back(job_optional);
  }
}

void GraphGenerationController::generate(
    const GenStartedCallback& gen_started_callback,
    const GenFinishedCallback& gen_finished_callback) {
  std::mutex callback_mutex;

  std::atomic<int> current_jobs_count = graphs_count_;
  for (int i = 0; i < graphs_count_; i++) {
    jobs_.emplace_back([i, &gen_started_callback, &gen_finished_callback,
                        &current_jobs_count, &callback_mutex,
                        &graph_generator = graph_generator_]() {
      {
        const std::lock_guard lock(callback_mutex);
        gen_started_callback(i);
      }

      std::unique_ptr<IGraph> graph = graph_generator.generate();

      {
        const std::lock_guard lock(callback_mutex);
        gen_finished_callback(i, std::move(graph));
      }

      current_jobs_count--;
    });
  }

  for (auto& worker : workers_) {
    worker.start();
  }

  while (current_jobs_count > 0) {
  }

  for (auto& worker : workers_) {
    worker.stop();
  }
}
}  // namespace uni_course_cpp
