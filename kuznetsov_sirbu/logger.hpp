#pragma once
#include <fstream>
#include <mutex>
#include <string>

namespace uni_course_cpp {
class Logger {
 public:
  static Logger& get_instance();
  void log(const std::string& string);

  Logger(const Logger&) = delete;
  void operator=(const Logger&) = delete;
  Logger(Logger&& other) = delete;
  void operator=(Logger&& other) = delete;

 private:
  Logger();
  ~Logger() { log_stream_.close(); };

  std::ofstream log_stream_;
  std::mutex log_control_;
};
}  // namespace uni_course_cpp
