#include "logger.hpp"
#include "config.hpp"

namespace uni_course_cpp {
std::string get_current_date_time() {
  const auto date_time = std::chrono::system_clock::now();
  const auto date_time_t = std::chrono::system_clock::to_time_t(date_time);
  std::stringstream date_time_string;
  date_time_string << std::put_time(std::localtime(&date_time_t),
                                    "%Y.%m.%d %H:%M:%S");
  return date_time_string.str();
}

Logger& Logger::get_logger() {
  static Logger logger;
  return logger;
}

Logger::Logger() : log_stream_(config::kLogFilePath) {}

void Logger::log(const std::string& string) {
  const auto date_time_string = get_current_date_time() + " " + string;
  std::cout << date_time_string << std::endl;
  log_stream_ << date_time_string << std::endl;
}
}  // namespace uni_course_cpp
