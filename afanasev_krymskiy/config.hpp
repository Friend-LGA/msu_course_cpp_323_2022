#pragma once

#include <string>

namespace uni_course_cpp {
namespace config {
static constexpr float kGreenEdgesProbability = 0.1;
static constexpr float kRedEdgesProbability = 0.33;
inline constexpr const char* kTempDirectoryPath = "./temp/";
inline constexpr const char* kLogFilename = "log.txt";
inline const std::string kLogFilePath =
    kTempDirectoryPath + std::string(kLogFilename);
}  // namespace config
}  // namespace uni_course_cpp
