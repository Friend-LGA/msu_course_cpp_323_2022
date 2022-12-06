#include "graph_generation_controller.hpp"
#include <atomic>
#include <cassert>

namespace uni_course_cpp {

namespace {
using JobCallBack = std::function<void()>;
}

void GraphGenerationController::Worker::start() {
  assert(state_ == State::Idle);
  state_ = State::Working;

  thread_ = std::thread([this]() {
    while (true) {
      if (state_ == State::ShouldTerminate) {
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

GraphGenerationController::GraphGenerationController(
    int threads_count,
    int graphs_count,
    GraphGenerator::Params&& params)
    : threads_count_(threads_count),
      graphs_count_(graphs_count),
      graph_generator_(GraphGenerator(std::move(params))) {
  const auto get_job_callback = [this]() -> std::optional<JobCallBack> {
    const std::lock_guard<std::mutex> guard(jobs_mutex_);
    if (!jobs_.empty()) {
      auto job = jobs_.front();
      jobs_.pop_front();
      return job;
    }

    return std::nullopt;
  };

  for (int i = 0; i < threads_count_; i++) {
    workers_.emplace_back(get_job_callback);
  }
}

void GraphGenerationController::Worker::stop() {
  assert(state_ == State::Working);
  state_ = State::ShouldTerminate;
  thread_.join();
  state_ = State::Idle;
}

GraphGenerationController::Worker::~Worker() {
  assert(state_ == State::Idle);
}

void GraphGenerationController::generate(
    const GenStartedCallback& gen_started_callback,
    const GenFinishedCallback& gen_finished_callback) {
  std::atomic<int> graphs_ready_count(graphs_count_);

  std::mutex log_mutex;
  for (int i = 0; i < graphs_count_; i++) {
    jobs_.emplace_back([&gen_started_callback, &gen_finished_callback,
                        &graphs_ready_count, &log_mutex, i, this]() {
      {
        const std::lock_guard<std::mutex> guard(log_mutex);
        gen_started_callback(i);
      }
      auto graph = graph_generator_.generate();
      {
        const std::lock_guard<std::mutex> guard(log_mutex);
        gen_finished_callback(i, std::move(graph));
      }
      graphs_ready_count -= 1;
    });
  }

  for (auto& worker : workers_) {
    worker.start();
  }

  while (graphs_ready_count != 0) {
  }

  for (auto& worker : workers_) {
    worker.stop();
  }
}

}  // namespace uni_course_cpp