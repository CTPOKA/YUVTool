#pragma once

#include "BMPImage.h"
#include "YUVVideoIO.h"
#include "YUVFrame.h"

class VideoHandler
{
public:
    static void attachImage(YUVVideoIO &yuvVideo, const BMPImage &image, int x, int y, float opacity);

private:
    static void mergeImageColorChannel(YUVFrame &frame,
                                  const YUVFrame &image,
                                  int xImage,
                                  int yImage,
                                  float opacity,
                                  uint8_t *YUVFrame::*channel);
};
