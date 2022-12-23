#pragma once
#include <fstream>
#include <string>

class Logger {
 public:
  static Logger& get_logger();
  void log(const std::string& str);

 private:
  Logger();
  ~Logger() = default;
  Logger(const Logger& other) = delete;
  void operator=(const Logger& other) = delete;
  Logger(Logger&& other) = delete;
  void operator=(Logger&& other) = delete;

  std::ofstream output_fstream_;
};
