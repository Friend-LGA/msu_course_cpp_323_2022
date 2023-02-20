#include "graph_generation_controller.hpp"
#include <cassert>
#include <mutex>
#include <thread>

namespace uni_course_cpp {

GraphGenerationController::GraphGenerationController(
    int threads_num,
    int graphs_num,
    const GraphGenerator::Params& graph_generator_params)
    : threads_num_(threads_num),
      graphs_num_(graphs_num),
      graph_generator_(graph_generator_params) {
  for (int i = 0; i < threads_num_; i++) {
    workers_.emplace_back([&jobs_ = jobs_, &mutex_jobs_ = mutex_jobs_,
                           this]() -> std::optional<JobCallback> {
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

void GraphGenerationController::Worker::start() {
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

void GraphGenerationController::Worker::stop() {
  assert(state_ == State::Working && "Worker has already stopped");
  state_ = State::ShouldTerminate;
  thread_.join();
}

GraphGenerationController::Worker::~Worker() {
  if (state_ == State::Working) {
    stop();
  }
}

void GraphGenerationController::generate(
    const GenStartedCallback& gen_started_callback,
    const GenFinishedCallback& gen_finished_callback) {
  for (int i = 0; i < graphs_num_; i++) {
    jobs_.emplace_back([&gen_started_callback, &gen_finished_callback,
                        &graph_generator_ = graph_generator_,
                        &graphs_generated_ = graphs_generated_, i, this]() {
      {
        const std::lock_guard lock(mutex_start_);
        gen_started_callback(i);
      }
      auto graph = graph_generator_.generate();
      {
        const std::lock_guard lock(mutex_finish_);
        gen_finished_callback(i, std::move(graph));
      }
      graphs_generated_++;
    });
  }

  for (auto& worker : workers_) {
    worker.start();
  }
  while (graphs_generated_ != graphs_num_) {
  }
  for (auto& worker : workers_) {
    worker.stop();
  }
}
}  // namespace uni_course_cpp
