#pragma once

#include <functional>
#include <list>
#include <optional>
#include <queue>
#include <thread>
#include "graph_traverser.hpp"
#include "interfaces/i_graph.hpp"
#include "interfaces/i_worker.hpp"

namespace uni_course_cpp {

class GraphTraversalController {
 public:
  using GenStartedCallback = std::function<void(int index)>;
  using GenFinishedCallback =
      std::function<void(int, const std::vector<GraphTraverser::GraphPath>&)>;

  GraphTraversalController(
      int threads_count,
      const std::vector<std::unique_ptr<uni_course_cpp::IGraph>>& graphs);

  void traverse_graphs(const GenStartedCallback& gen_started_callback,
                       const GenFinishedCallback& gen_finished_callback);

 private:
  using JobCallback = std::function<void()>;

  class Worker : public IWorker {
   public:
    using GetJobCallback = std::function<std::optional<JobCallback>()>;

    explicit Worker(const GetJobCallback& get_job_callback)
        : get_job_callback_(get_job_callback) {}

    void start() override;
    void stop() override;

    ~Worker();

   private:
    enum class State { Idle, Working, ShouldTerminate };

    std::thread thread_;
    GetJobCallback get_job_callback_;
    State state_ = State::Idle;
  };

  std::list<Worker> workers_;
  std::list<JobCallback> jobs_;
  const std::vector<std::unique_ptr<uni_course_cpp::IGraph>>& graphs_;
  std::mutex start_callback_mutex_;
  std::mutex finish_callback_mutex_;
  std::mutex get_job_mutex_;
};

}  // namespace uni_course_cpp
