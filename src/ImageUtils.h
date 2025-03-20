#pragma once

#include "BMPImage.h"
#include "YUVFrame.h"

#include <cmath>
#include <immintrin.h>

namespace ImageUtils {
YUVFrame BMPtoYUV420(const BMPImage &bmp);
}
