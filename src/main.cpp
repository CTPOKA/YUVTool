#include "ArgumentParser.h"
#include "BMPReader.h"
#include "FileSystemUtils.h"
#include "VideoHandler.h"

#include <chrono>
#include <iostream>
#include <string>

void printUsage(std::string str = "YUVTool")
{
    std::cout << "Usage: " << str
              << " <video_in> <video_out> <bmp> <width> <height> [-x X] [-y Y] [-o "
                 "OPACITY] [-f FORCE]\n";
}

int main(int argc, char *argv[])
{
    ArgumentParser parser(argc, argv);

    std::string inPath = parser.getInPath();
    std::string outPath = parser.getOutPath();
    std::string bmpPath = parser.getBMPPath();

    int width = parser.getWidth();
    int height = parser.getHeight();

    int x = parser.getIntOption("-x", 0);
    int y = parser.getIntOption("-y", 0);
    float opacity = parser.getFloatOption("-a", 1.0f);
    bool autoConfirm = parser.hasOption("--yes");

    if (inPath == outPath) {
        std::cerr << "Error: The program cannot process in place. Please specify different input "
                     "and output files.\n";
        return 1;
    }

    if (!FileSystemUtils::fileExists(bmpPath)) {
        std::cerr << "Error: BMP file not found\n";
        return 1;
    }

    BMPImage img;
    try {
        img = BMPReader::load(bmpPath);
    } catch (const std::exception &e) {
        std::cerr << "Error loading BMP: " << e.what() << "\n";
        return 1;
    }

    if (x + img.width() > width || y + img.height() > height) {
        std::cout << "Warning: Image exceeds video bounds!" << std::endl;
    }

    if (FileSystemUtils::fileExists(outPath)) {
        std::cout << "File " << outPath << " already exists. Overwrite? (y/n): ";

        if (!autoConfirm) {
            char response;
            std::cin >> response;
            if (response != 'y' && response != 'Y') {
                std::cout << "Aborted.\n";
                return 1;
            }
        } else {
            std::cout << "y" << std::endl;
        }
    }

    uintmax_t inputSize = FileSystemUtils::getFileSize(inPath);
    uintmax_t freeSpace = FileSystemUtils::getFreeSpace(outPath);
    if (inputSize > freeSpace) {
        std::cerr << "Error: Not enough disk space for output file.\n";
        return 1;
    }

    try {
        YUVVideoIO video(inPath, outPath, width, height);

        std::cout << "Processing has started" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        VideoHandler::attachImage(video, img, x, y, opacity);
        video.close();
        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Processing time: " << std::chrono::duration<double>(end - start).count()
                  << " s\n";
    } catch (const std::exception &e) {
        std::cerr << "Error processing video: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
