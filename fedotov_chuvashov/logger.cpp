#include "logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "config.hpp"

namespace uni_course_cpp {

Logger& Logger::get_logger() {
  static Logger logger;
  return logger;
}

std::string Logger::get_current_date_time() {
  const auto date_time = std::chrono::system_clock::now();
  const auto date_time_t = std::chrono::system_clock::to_time_t(date_time);
  std::stringstream date_time_string;
  date_time_string << std::put_time(std::localtime(&date_time_t),
                                    "%Y.%m.%d %H:%M:%S");
  return date_time_string.str();
}

Logger::Logger() : log_stream_(config::kLogFilePath, std::ofstream::app) {}

void Logger::log(const std::string& string) {
  const std::string& current_date_time = get_current_date_time();
  const std::lock_guard<std::mutex> lock(logger_mutex_);
  log_stream_ << get_current_date_time() << string;
  std::cout << get_current_date_time() << string;
}
}  // namespace uni_course_cpp
