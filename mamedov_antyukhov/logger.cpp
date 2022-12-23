#include "logger.hpp"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "config.hpp"

namespace {
std::string get_current_date_time() {
  const auto date_time = std::chrono::system_clock::now();
  const auto date_time_t = std::chrono::system_clock::to_time_t(date_time);
  std::stringstream date_time_string;
  date_time_string << std::put_time(std::localtime(&date_time_t),
                                    "%Y.%m.%d %H:%M:%S ");
  return date_time_string.str();
}
}  // namespace

namespace uni_course_cpp {

Logger& Logger::get_logger() {
  static Logger instance = Logger();
  return instance;
}

Logger::Logger() : log_file_(config::kLogFilePath) {}

void Logger::log(const std::string& string) {
  const std::string current_date_time = get_current_date_time();
  const std::lock_guard<std::mutex> log_lock(log_mutex_);
  log_file_ << current_date_time << string;
  std::cout << current_date_time << string;
}
};  // namespace uni_course_cpp
