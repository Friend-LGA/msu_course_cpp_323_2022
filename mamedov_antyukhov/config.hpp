#pragma once

#include <string>

namespace uni_course_cpp {
namespace config {

inline constexpr const char* kTempDirectoryPath = "./temp/";
inline constexpr const char* kLogFilename = "log.txt";
inline const std::string kLogFilePath =
    static_cast<std::string>(kTempDirectoryPath) +
    static_cast<std::string>(kLogFilename);

}  // namespace config
}  // namespace uni_course_cpp
