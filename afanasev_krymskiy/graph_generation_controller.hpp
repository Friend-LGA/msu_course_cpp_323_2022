#pragma once

#include <atomic>
#include <cassert>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
#include <thread>

#include "graph.hpp"
#include "graph_generator.hpp"

namespace uni_course_cpp {
class GraphGenerationController {
 public:
  using GenStartedCallback = std::function<void(int index)>;
  using GenFinishedCallback = std::function<void(int index, Graph&& graph)>;

  GraphGenerationController(int threads_count,
                            int graphs_count,
                            GraphGenerator::Params&& params);

  void generate(const GenStartedCallback& gen_started_callback,
                const GenFinishedCallback& gen_finished_callback);

 private:
  using JobCallback = std::function<void()>;

  class Worker {
   public:
    using GetJobCallback = std::function<std::optional<JobCallback>()>;

    explicit Worker(const GetJobCallback& get_job_callback)
        : get_job_callback_(get_job_callback) {}

    void start();
    void stop();

    ~Worker();

   private:
    enum class State { Idle, Working, ShouldTerminate };

    std::thread thread_;
    GetJobCallback get_job_callback_;
    State state_ = State::Idle;
  };

  std::list<Worker> workers_;
  std::list<JobCallback> jobs_;
  int threads_count_;
  int graphs_count_;
  std::mutex mutex_for_jobs_;
  GraphGenerator graph_generator_;
};
}  // namespace uni_course_cpp
