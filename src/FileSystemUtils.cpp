#include "FileSystemUtils.h"

#include <cstring>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>

bool FileSystemUtils::fileExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

uint32_t FileSystemUtils::getFileSize(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0) ? buffer.st_size : 0;
}

uint32_t FileSystemUtils::getFreeSpace(const std::string &path)
{
    struct statvfs stat;

    std::string dir = path.substr(0, path.find_last_of("/\\"));

    return (statvfs(dir.c_str(), &stat) == 0) ? stat.f_bavail * stat.f_frsize : 0;
}
