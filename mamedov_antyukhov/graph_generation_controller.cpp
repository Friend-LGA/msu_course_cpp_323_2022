#include "graph_generation_controller.hpp"
#include <atomic>
#include <cassert>

namespace uni_course_cpp {

void GraphGenerationController::Worker::start() {
  assert(state_ == State::Idle);
  state_ = State::Working;
  thread_ =
      std::thread([&state = state_, &get_job_callback = get_job_callback_]() {
        while (true) {
          if (state == State::ShouldTerminate) {
            state = State::Idle;
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
  const auto get_job_callback = [&jobs = jobs_, &jobs_mutex = jobs_mutex_]()
      -> std::optional<GraphGenerationController::JobCallback> {
    const std::lock_guard<std::mutex> jobs_lock(jobs_mutex);
    if (!jobs.empty()) {
      auto job = jobs.front();
      jobs.pop();
      return job;
    }

    return std::nullopt;
  };
  for (int i = 0; i < graphs_count_; ++i) {
    workers_.emplace_back(get_job_callback);
  }
}

void GraphGenerationController::generate(
    const GenStartedCallback& gen_started_callback,
    const GenFinishedCallback& gen_finished_callback) {
  std::atomic<int> graphs_created_num = graphs_count_;
  std::mutex gen_mutex;
  for (int i = 0; i < graphs_count_; ++i) {
    jobs_.push([&gen_started_callback, &gen_finished_callback,
                &graph_generator = graph_generator_, i, &graphs_created_num,
                &gen_mutex]() {
      {
        const std::lock_guard<std::mutex> gen_start_lock(gen_mutex);
        gen_started_callback(i);
      }
      auto graph = graph_generator.generate();
      {
        const std::lock_guard<std::mutex> gen_finish_lock(gen_mutex);
        gen_finished_callback(i, std::move(graph));
      }
      --graphs_created_num;
    });
  }

  for (auto& worker : workers_) {
    worker.start();
  }

  while (graphs_created_num) {
  }

  for (auto& worker : workers_) {
    worker.stop();
  }
}
}  // namespace uni_course_cpp
