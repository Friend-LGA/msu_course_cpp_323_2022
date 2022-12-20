#include "graph_logger.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "config.hpp"

namespace {

std::string get_current_date_time() {
  const auto date_time = std::chrono::system_clock::now();
  const auto date_time_t = std::chrono::system_clock::to_time_t(date_time);
  std::stringstream date_time_string;
  date_time_string << std::put_time(std::localtime(&date_time_t),
                                    "%Y.%m.%d %H:%M:%S");
  return date_time_string.str() + " ";
}

}  // namespace

Logger::Logger()
    : output_fstream_(std::ofstream(config::kLogFilePath, std::ios::app)) {}

Logger& Logger::get_logger() {
  static Logger instance;
  return instance;
}

void Logger::log(const std::string& str) {
  const auto& current_date_time = get_current_date_time();
  const std::lock_guard<std::mutex> guard(log_mutex_);
  std::cout << current_date_time + str << std::endl;
  output_fstream_ << current_date_time + str << std::endl;
}
