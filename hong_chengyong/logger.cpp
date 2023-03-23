#include "logger.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include "config.hpp"

namespace {
std::string getCurrentDateTime() {
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
  const std::lock_guard lock(logger_mutex_);
  std::string log_info = getCurrentDateTime();
  log_info += ": " + string;
  std::cout << log_info;
  file_stream_ << log_info;
}

Logger& Logger::getLogger() {
  static Logger logger;
  return logger;
}

Logger::~Logger() {
  file_stream_.close();
}

Logger::Logger()
    : file_stream_(config::TEMP_DIRECTORY_PATH + config::LOG_FILE_NAME) {
  if (!file_stream_.is_open()) {
    throw "Error during log file opening";
  }
}
}  // namespace uni_course_cpp