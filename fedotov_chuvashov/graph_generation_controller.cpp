#include "graph_generation_controller.hpp"
#include <atomic>
#include <cassert>
#include <mutex>

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
      graph_generator_(GraphGenerator(std::move(graph_generator_params))) {
  const auto worker_callback = [&controller_mutex = controller_mutex_,
                                &jobs = jobs_]() -> std::optional<JobCallback> {
    const std::lock_guard<std::mutex> lock(controller_mutex);
    if (!jobs.empty()) {
      auto job = jobs.front();
      jobs.pop_front();
      return job;
    }
    return std::nullopt;
  };

  for (int i = 0; i < threads_count_; ++i) {
    workers_.emplace_back(worker_callback);
  }
}

void GraphGenerationController::generate(
    const GenStartedCallback& gen_started_callback,
    const GenFinishedCallback& gen_finished_callback) {
  std::atomic<int> graphs_generated = 0;

  for (int i = 0; i < graphs_count_; ++i) {
    jobs_.emplace_back([&gen_started_callback, &gen_finished_callback,
                        &graphs_generated,
                        &controller_mutex = controller_mutex_,
                        &graph_generator = graph_generator_, i]() {
      {
        const std::lock_guard<std::mutex> lock_callback(controller_mutex);
        gen_started_callback(i);
      }
      auto graph = graph_generator.generate();
      {
        const std::lock_guard<std::mutex> lock_callback(controller_mutex);
        gen_finished_callback(i, std::move(graph));
      }
      ++graphs_generated;
    });
  }

  for (auto& worker : workers_) {
    worker.start();
  }

  while (graphs_generated < graphs_count_) {
  }

  for (auto& worker : workers_) {
    worker.stop();
  }
}
}  // namespace uni_course_cpp
