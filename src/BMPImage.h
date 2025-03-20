#pragma once

#include <cmath>
#include <cstdint>
#include <vector>

#pragma pack(push, 1)
struct BITMAPFILEHEADER
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BITMAPINFOHEADER
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};

struct RGBPixel
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};
#pragma pack(pop)

struct BMPImage
{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    std::vector<RGBPixel> pixels;

    BMPImage() = default;

    BMPImage(BITMAPFILEHEADER fhdr, BITMAPINFOHEADER ihdr)
        : fileHeader(fhdr)
        , infoHeader(ihdr)
        , pixels(width() * height())
    {}

    int width() const { return infoHeader.biWidth; }
    int height() const { return abs(infoHeader.biHeight); }

    RGBPixel *operator[](int y)
    {
        return infoHeader.biHeight > 0 ? pixels.data() + width() * (height() - y - 1)
                                       : pixels.data() + y * width();
    }
    const RGBPixel *operator[](int y) const { return pixels.data() + y * width(); }

    BMPImage(const BMPImage &) = default;
    BMPImage &operator=(const BMPImage &) = default;

    BMPImage(BMPImage &&) noexcept = default;
    BMPImage &operator=(BMPImage &&) noexcept = default;
};
