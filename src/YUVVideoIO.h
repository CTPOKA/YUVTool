#pragma once

#include "YUVFrame.h"

#include <cstdint>
#include <cstdio>
#include <string>

class YUVVideoIO
{
public:
    YUVVideoIO(const std::string &inputFilename,
               const std::string &outputFilename,
               int width,
               int height);

    ~YUVVideoIO();

    bool getNextFrame(YUVFrame& frame);
    void writeFrame(const YUVFrame& frame);
    void close();

    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    const static int BUFFER_SIZE = 1024 * 1024;

    std::string inputFilename;
    std::string outputFilename;
    int m_width;
    int m_height;
    int frameSize;

    FILE *fileRead;
    FILE *fileWrite;

    uint8_t *readBuffer;
    uint8_t *writeBuffer;
};
