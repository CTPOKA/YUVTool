#pragma once

#include <cstdint>
#include <vector>

struct YUVFrame
{
    std::vector<uint8_t> data;
    uint8_t *Y;
    uint8_t *U;
    uint8_t *V;
    int width;
    int height;

    YUVFrame(int w, int h)
        : width(w & ~1)
        , height(h & ~1)
    {
        int frameSize = width * height + (width / 2) * (height / 2) * 2;

        data.resize(frameSize);
        Y = data.data();
        U = Y + width * height;
        V = U + (width / 2) * (height / 2);
    }

    YUVFrame(YUVFrame &&other) noexcept
        : data(std::move(other.data))
        , Y(other.Y)
        , U(other.U)
        , V(other.V)
        , width(other.width)
        , height(other.height)
    {
        other.Y = other.U = other.V = nullptr;
    }

    YUVFrame &operator=(YUVFrame &&other) noexcept
    {
        if (this != &other) {
            data = std::move(other.data);
            Y = other.Y;
            U = other.U;
            V = other.V;
            width = other.width;
            height = other.height;
            other.Y = other.U = other.V = nullptr;
        }
        return *this;
    }

    YUVFrame(const YUVFrame &) = default;
    YUVFrame &operator=(const YUVFrame &) = default;
};
