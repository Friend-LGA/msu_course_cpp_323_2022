#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include "config.hpp"
#include "logger.hpp"

namespace {
std::string get_current_date_time() {
  const auto date_time = std::chrono::system_clock::now();
  const auto date_time_t = std::chrono::system_clock::to_time_t(date_time);
  std::stringstream date_time_string;
  date_time_string << std::put_time(std::localtime(&date_time_t),
                                    "%Y.%m.%d %H:%M:%S");
  return date_time_string.str();
}
}  // namespace

namespace uni_course_cpp {
void Logger::log(const std::string& string) {
  const auto result = get_current_date_time() + string;
  const std::lock_guard lock(logger_mutex_);
  std::cout << result << std::endl;
  log_ << result << std::endl;
}

Logger& Logger::get_logger() {
  static Logger logger;
  return logger;
}

Logger::Logger() : log_(config::kLogFilePath, std::ios::app) {}
}  // namespace uni_course_cpp
