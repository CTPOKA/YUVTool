#include "ImageUtils.h"
#include "AVXVector.h"
#include "BMPImage.h"
#include "YUVFrame.h"

#include <thread>
#include <vector>

YUVFrame ImageUtils::BMPtoYUV420(const BMPImage &bmp)
{
    int width = bmp.width() & ~1;
    int height = bmp.height() & ~1;
    YUVFrame yuv(width, height);

    int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    auto processRowRange = [&](int startY, int endY) {
        for (int y = startY; y < endY; ++y) {
            int y2 = bmp.infoHeader.biHeight < 0 ? height - y - 1 : y;
            for (int x = 0; x < (width & ~31); x += 32) {
                const uint8_t *data = reinterpret_cast<const uint8_t *>(&bmp[y2][x]);

                // Загружаем 32 пикселя (96 байтов) в три AVX-вектора
                __m256i lo1 = _mm256_loadu_si256((__m256i *) data);
                __m128i hi1 = _mm_loadu_si128((__m128i *) data + 3);
                __m256i vec1 = _mm256_inserti128_si256(lo1, hi1, 1);

                __m256i lo2 = _mm256_loadu_si256((__m256i *) (data + 16));
                __m128i hi2 = _mm_loadu_si128((__m128i *) data + 4);
                __m256i vec2 = _mm256_inserti128_si256(lo2, hi2, 1);

                __m256i lo3 = _mm256_loadu_si256((__m256i *) (data + 32));
                __m128i hi3 = _mm_loadu_si128((__m128i *) data + 5);
                __m256i vec3 = _mm256_inserti128_si256(lo3, hi3, 1);

                // Маска для выделения компонент пикселей (R, G, B)
                static const __m256i mask = _mm256_setr_epi8(0, 3, 6, 9, 12, 15, 1, 4, 7, 10, 13, 2, 5, 8, 11, 14,
                                                             0, 3, 6, 9, 12, 15, 1, 4, 7, 10, 13, 2, 5, 8, 11, 14);

                vec1 = _mm256_shuffle_epi8(vec1, mask);
                vec2 = _mm256_shuffle_epi8(vec2, mask);
                vec3 = _mm256_shuffle_epi8(vec3, mask);

                // Разделяем RGB-каналы
                __m256i R, G, B, T;
                T = _mm256_alignr_epi8(vec2, vec2, 11);
                R = _mm256_alignr_epi8(vec3, T, 11);
                B = _mm256_alignr_epi8(vec1, R, 11);
                G = _mm256_alignr_epi8(vec2, B, 11);

                R = _mm256_alignr_epi8(R, vec1, 11);
                B = _mm256_alignr_epi8(B, vec2, 11);
                G = _mm256_alignr_epi8(G, vec3, 11);

                // Выравниваем
                B = _mm256_alignr_epi8(B, B, 10);
                G = _mm256_alignr_epi8(G, G, 5);

                AVX_uint8 Rvec(R), Gvec(G), Bvec(B);
                AVX_uint8 Y = Rvec * 0.299f + Gvec * 0.587f + Bvec * 0.114f;
                AVX_uint8 U = 128 - Rvec * 0.147f + Bvec * 0.436f - Gvec * 0.289f;
                AVX_uint8 V = 128 - Gvec * 0.515f + Rvec * 0.615f - Bvec * 0.1f;

                // Записываем компоненту Y
                Y.store(&yuv.Y[y * width + x]);

                if ((y & 1) == 0) {
                    int index = (y / 2) * (width / 2) + (x / 2);

                    // Берем средние значения U и V
                    U = _mm256_avg_epu8(U, _mm256_alignr_epi8(U, U, 1));
                    U = _mm256_and_si256(U, _mm256_set1_epi16(0xFF));
                    V = _mm256_avg_epu8(V, _mm256_alignr_epi8(V, V, 1));
                    V = _mm256_and_si256(V, _mm256_set1_epi16(0xFF));

                    // Упаковываем результат в __m128i
                    __m256i hiU = _mm256_castsi128_si256(_mm256_extracti128_si256(U, 1));
                    __m128i packedU = _mm256_castsi256_si128(_mm256_packus_epi16(U, hiU));
                    __m256i hiV = _mm256_castsi128_si256(_mm256_extracti128_si256(V, 1));
                    __m128i packedV = _mm256_castsi256_si128(_mm256_packus_epi16(V, hiV));

                    // Записываем компоненты U и V
                    _mm_storeu_si128(reinterpret_cast<__m128i *>(&yuv.U[index]), packedU);
                    _mm_storeu_si128(reinterpret_cast<__m128i *>(&yuv.V[index]), packedV);
                }
            }

            // Обрабатываем хвост пикселей без AVX
            for (int x = (width & ~31); x < width; ++x) {
                int uvIndex = (y >> 1) * (width >> 1) + (x >> 1);

                const RGBPixel &pxl = bmp[y2][x];

                uint8_t Y = static_cast<uint8_t>(0.299 * pxl.red + 0.587 * pxl.green
                                                 + 0.114 * pxl.blue);
                uint8_t U = static_cast<uint8_t>((pxl.blue - Y) * 0.565 + 128);
                uint8_t V = static_cast<uint8_t>((pxl.red - Y) * 0.713 + 128);

                yuv.Y[y * width + x] = Y;
                yuv.U[uvIndex] += U >> 2;
                yuv.V[uvIndex] += V >> 2;
            }
        }
    };

    int rowsPerThread = height / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int startY = i * rowsPerThread;
        int endY = (i == numThreads - 1) ? height : startY + rowsPerThread;
        threads.emplace_back(processRowRange, startY, endY);
    }

    for (auto &t : threads) {
        t.join();
    }

    return yuv;
}
