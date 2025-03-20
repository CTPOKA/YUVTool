#pragma once

#include "BMPImage.h"

#include <string>

class BMPReader
{
public:
    static BMPImage load(const std::string &filename);
};
