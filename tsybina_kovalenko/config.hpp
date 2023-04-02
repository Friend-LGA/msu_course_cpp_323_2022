#pragma once

#include <string>

namespace uni_course_cpp {
namespace config {
inline constexpr const char* kTempDirectoryPath = "./temp/";

inline const std::string kLogFilename = "log.txt";
inline const std::string kLogFilePath = kTempDirectoryPath + kLogFilename;

}  // namespace config
}  // namespace uni_course_cpp
