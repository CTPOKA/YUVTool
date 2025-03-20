#pragma once

#include <cstdint>
#include <immintrin.h>

struct AVX_uint8
{
    union {
        __m256i vec;
        uint8_t bytes[32];
    };

    AVX_uint8() = default;

    explicit AVX_uint8(int value) { vec = _mm256_set1_epi8(value); }

    explicit AVX_uint8(const uint8_t *ptr)
    {
        vec = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(ptr));
    }

    AVX_uint8(__m256i data)
        : vec(data)
    {}

    void store(void *ptr) { _mm256_storeu_si256(reinterpret_cast<__m256i *>(ptr), vec); }

    AVX_uint8 operator+(const __m256i &other) const { return _mm256_adds_epu8(vec, other); }
    AVX_uint8 operator+(int value) const { return _mm256_adds_epu8(vec, AVX_uint8(value)); }

    AVX_uint8 &operator+=(const __m256i &other)
    {
        vec = vec + other;
        return *this;
    }

    //Приблизительное умножение uint8_t на float
    AVX_uint8 &operator*=(float scalar)
    {
        __m256i scalarVec = _mm256_set1_epi16(256 * scalar);

        __m256i &high = vec;
        __m256i low = _mm256_slli_si256(high, 1);

        high = _mm256_slli_si256(_mm256_mulhi_epu16(high, scalarVec), 1);
        low = _mm256_mulhi_epu16(low, scalarVec);

        static const __m256i mask = _mm256_set1_epi16(255);
        vec = _mm256_blendv_epi8(high, low, mask);

        return *this;
    }

    AVX_uint8 operator*(float scalar) const
    {
        AVX_uint8 res = *this;
        res *= scalar;
        return res;
    }

    operator __m256i() const { return vec; }
};

inline AVX_uint8 operator-(const __m256i &lhs, const AVX_uint8 &rhs)
{
    return _mm256_subs_epu8(lhs, rhs);
}

inline AVX_uint8 operator-(const int &lhs, const AVX_uint8 &rhs)
{
    return _mm256_subs_epu8(AVX_uint8(lhs), rhs);
}
