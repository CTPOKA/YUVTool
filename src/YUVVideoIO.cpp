#include "YUVVideoIO.h"
#include <stdexcept>
#include <cstring>

YUVVideoIO::YUVVideoIO(const std::string &inputFilename,
                       const std::string &outputFilename,
                       int width,
                       int height)
    : inputFilename(inputFilename)
    , outputFilename(outputFilename)
    , m_width(width)
    , m_height(height)
    , fileRead(nullptr)
    , fileWrite(nullptr)
{
    fileRead = fopen(inputFilename.c_str(), "rb");
    if (!fileRead) {
        throw std::runtime_error("Failed to open input file");
    }

    fileWrite = fopen(outputFilename.c_str(), "wb");
    if (!fileWrite) {
        throw std::runtime_error("Failed to open output file");
    }

    readBuffer = new uint8_t[BUFFER_SIZE];
    writeBuffer = new uint8_t[BUFFER_SIZE];

    setvbuf(fileRead, reinterpret_cast<char *>(readBuffer), _IOFBF, BUFFER_SIZE);
    setvbuf(fileWrite, reinterpret_cast<char *>(writeBuffer), _IOFBF, BUFFER_SIZE);

    frameSize = m_width * m_height + (m_width / 2) * (m_height / 2) * 2;
}

YUVVideoIO::~YUVVideoIO()
{
    close();
    delete[] readBuffer;
    delete[] writeBuffer;
}

bool YUVVideoIO::getNextFrame(YUVFrame &frame)
{
    size_t bytesRead = fread(frame.data.data(), 1, frame.data.size(), fileRead);

    if (bytesRead == 0)
        return false;

    if (bytesRead != frame.data.size()) {
        throw std::runtime_error("Error reading YUV frame");
    }

    return true;
}

void YUVVideoIO::writeFrame(const YUVFrame& frame)
{
    if (fwrite(frame.data.data(), 1, frame.data.size(), fileWrite) != frame.data.size()) {
        throw std::runtime_error("Failed to write frame");
    }
}

void YUVVideoIO::close()
{
    if (fileRead) {
        fclose(fileRead);
        fileRead = nullptr;
    }

    if (fileWrite) {
        fclose(fileWrite);
        fileWrite = nullptr;
    }
}
