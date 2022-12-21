#pragma once

#include <functional>
#include <optional>

namespace uni_course_cpp {
using JobCallback = std::function<void()>;
using GetJobCallback = std::function<std::optional<JobCallback>()>;
class IWorker {
 public:
  virtual ~IWorker(){};
  virtual void start() = 0;
  virtual void stop() = 0;
};
}  // namespace uni_course_cpp
