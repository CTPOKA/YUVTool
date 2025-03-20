#pragma once

#include <cstdint>
#include <string>

namespace FileSystemUtils {
bool fileExists(const std::string &path);
uint32_t getFileSize(const std::string &path);
uint32_t getFreeSpace(const std::string &path);
} // namespace FileSystemUtils
