cmake_minimum_required(VERSION 3.16)

project(YUVTool LANGUAGES CXX)

add_compile_options(-mavx2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(YUVTool
    src/main.cpp src/ImageUtils.h
    src/BMPReader.h src/BMPReader.cpp
    src/VideoHandler.h src/VideoHandler.cpp
    src/BMPImage.h
    src/YUVFrame.h
    src/YUVVideoIO.h src/YUVVideoIO.cpp
    src/AVXVector.h
    src/ImageUtils.cpp
    src/FileSystemUtils.h src/FileSystemUtils.cpp
    src/ArgumentParser.h src/ArgumentParser.cpp
)
