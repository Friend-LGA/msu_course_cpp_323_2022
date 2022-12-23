#pragma once

#include <functional>
#include <list>
#include <optional>
#include <queue>
#include <thread>
#include "graph_generator.hpp"
#include "interfaces/i_worker.hpp"

namespace uni_course_cpp {
class GraphGenerationController {
 public:
  using GenStartedCallback = std::function<void(int index)>;
  using GenFinishedCallback =
      std::function<void(int index, std::unique_ptr<IGraph> graph)>;

  GraphGenerationController(int threads_count,
                            int graphs_count,
                            GraphGenerator::Params&& graph_generator_params);

  void generate(const GenStartedCallback& gen_started_callback,
                const GenFinishedCallback& gen_finished_callback);

 private:
  class Worker : public IWorker {
   public:
    explicit Worker(const GetJobCallback& get_job_callback)
        : get_job_callback_(get_job_callback) {}

    void start() override;
    void stop() override;

    ~Worker() override;

   private:
    enum class State { Idle, Working, ShouldTerminate };

    std::thread thread_;
    GetJobCallback get_job_callback_;
    State state_ = State::Idle;
  };

  std::list<Worker> workers_;
  std::queue<JobCallback> jobs_;
  int threads_count_;
  int graphs_count_;
  GraphGenerator graph_generator_;
  std::mutex jobs_mutex_;
};
}  // namespace uni_course_cpp
