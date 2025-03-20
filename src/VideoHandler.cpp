#include "VideoHandler.h"
#include "AVXVector.h"
#include "ImageUtils.h"
#include <algorithm>
#include <cstring>
#include <immintrin.h>

void VideoHandler::attachImage(
    YUVVideoIO &yuvVideo, const BMPImage &bmpImage, int x, int y, float opacity)
{
    YUVFrame YUVimage = ImageUtils::BMPtoYUV420(bmpImage);
    int imageSizeY = YUVimage.width * YUVimage.height;
    int imageSizeUV = imageSizeY / 4;

    for (int i = 0; i < imageSizeY; ++i) {
        YUVimage.Y[i] *= opacity;
    }
    for (int i = 0; i < imageSizeUV; ++i) {
        YUVimage.U[i] *= opacity;
        YUVimage.V[i] *= opacity;
    }

    YUVFrame frame(yuvVideo.width(), yuvVideo.height());
    while (yuvVideo.getNextFrame(frame)) {
        VideoHandler::mergeImageColorChannel(frame, YUVimage, x, y, 1 - opacity, &YUVFrame::Y);
        VideoHandler::mergeImageColorChannel(frame, YUVimage, x, y, 1 - opacity, &YUVFrame::U);
        VideoHandler::mergeImageColorChannel(frame, YUVimage, x, y, 1 - opacity, &YUVFrame::V);

        yuvVideo.writeFrame(frame);
    }
}

void VideoHandler::mergeImageColorChannel(YUVFrame &frame,
                                          const YUVFrame &image,
                                          int xImage,
                                          int yImage,
                                          float opacity,
                                          uint8_t *YUVFrame::*channel)
{
    uint8_t *frameData = frame.*channel;
    const uint8_t *imageData = image.*channel;

    bool isChroma = (channel == &YUVFrame::U || channel == &YUVFrame::V);

    int frameWidth = frame.width >> (int) isChroma;
    int frameHeight = frame.height >> (int) isChroma;
    int imageWidth = image.width >> (int) isChroma;
    int imageHeight = image.height >> (int) isChroma;
    xImage >>= (int) isChroma;
    yImage >>= (int) isChroma;

    int maxOverlayWidth = std::min(frameWidth - xImage, imageWidth);
    int maxOverlayHeight = std::min(frameHeight - yImage, imageHeight);

    for (int y = 0; y < maxOverlayHeight; ++y) {
        int frameY = y + yImage;

        for (int x = 0; x < (maxOverlayWidth & ~31); x += 32) {
            int frameX = x + xImage;
            AVX_uint8 frmVec(&frameData[frameY * frameWidth + frameX]);
            AVX_uint8 imgVec(&imageData[y * imageWidth + x]);

            frmVec *= opacity;
            (frmVec + imgVec).store(&frameData[frameY * frameWidth + frameX]);
        }

        int tailIndex = maxOverlayWidth & ~31;
        int frameTailX = tailIndex + xImage;
        AVX_uint8 tail(&frameData[frameY * frameWidth + frameTailX]);
        AVX_uint8 imgVec(&imageData[y * imageWidth + tailIndex]);
        tail *= opacity;
        tail += imgVec;

        for (int x = tailIndex; x < maxOverlayWidth; x += 2) {
            int frameX = x + xImage;
            if (frameX + 1 >= frameWidth)
                break;
            frameData[frameY * frameWidth + frameX] = tail.bytes[x - tailIndex];
            frameData[frameY * frameWidth + frameX + 1] = tail.bytes[x - tailIndex + 1];
        }
    }
}
