#pragma once
#include <functional>
#include <optional>
#include <thread>

namespace uni_course_cpp {

using JobCallback = std::function<void()>;

class IWorker {
  using GetJobCallback = std::function<std::optional<JobCallback>()>;

 public:
  virtual ~IWorker(){};

  virtual void start() = 0;
  virtual void stop() = 0;
};

}  // namespace uni_course_cpp