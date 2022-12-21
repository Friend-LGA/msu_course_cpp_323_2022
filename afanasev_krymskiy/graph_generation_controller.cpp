#include <atomic>
#include <cassert>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

#include "graph_generation_controller.hpp"
#include "interfaces/i_worker.hpp"

namespace uni_course_cpp {
GraphGenerationController::GraphGenerationController(
    int threads_count,
    int graphs_count,
    GraphGenerator::Params&& params)
    : threads_count_(threads_count),
      graphs_count_(graphs_count),
      graph_generator_(std::move(params)) {
  const auto job_optional = [&mutex_for_jobs = mutex_for_jobs_,
                             &jobs = jobs_]() -> std::optional<JobCallback> {
    const std::lock_guard lock(mutex_for_jobs);
    if (!jobs.empty()) {
      const auto item = jobs.back();
      jobs.pop_back();
      return item;
    }
    return std::nullopt;
  };

  for (int i = 0; i < threads_count; ++i)
    workers_.emplace_back(job_optional);
}
void GraphGenerationController::generate(
    const GenStartedCallback& gen_started_callback,
    const GenFinishedCallback& gen_finished_callback) {
  std::atomic<int> wait_for_jobs_count = graphs_count_;

  std::mutex callback_mutex;
  for (int index = 0; index < graphs_count_; ++index)
    jobs_.emplace_back([&gen_started_callback, &gen_finished_callback,
                        &wait_for_jobs_count, &callback_mutex, index,
                        &graph_generator = graph_generator_]() {
      {
        const std::lock_guard lock(callback_mutex);
        gen_started_callback(index);
      }
      auto graph = graph_generator.generate();
      {
        const std::lock_guard lock(callback_mutex);
        gen_finished_callback(index, std::move(graph));
      }
      --wait_for_jobs_count;
    });

  for (auto& worker : workers_)
    worker.start();

  while (wait_for_jobs_count)
    ;

  for (auto& worker : workers_)
    worker.stop();
}

void GraphGenerationController::Worker::start() {
  assert(state_ == State::Idle);

  state_ = State::Working;
  thread_ =
      std::thread([&state = state_, &get_job_callback = get_job_callback_]() {
        while (true) {
          if (state == State::ShouldTerminate)
            return;
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
  if (state_ == State::Working)
    stop();
}
}  // namespace uni_course_cpp
