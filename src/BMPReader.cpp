#include "BMPReader.h"

#include <fstream>
#include <stdexcept>

BMPImage BMPReader::load(const std::string &filename)
{
    BITMAPFILEHEADER fhdr;
    BITMAPINFOHEADER ihdr;
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open BMP file");

    file.read(reinterpret_cast<char *>(&fhdr), sizeof(fhdr));
    file.read(reinterpret_cast<char *>(&ihdr), sizeof(ihdr));

    if (fhdr.bfType != 0x4D42)
        throw std::runtime_error("Invalid BMP file");

    BMPImage image(fhdr, ihdr);

    if (image.infoHeader.biBitCount != 24)
        throw std::runtime_error("Only 24-bit BMPs are supported!");

    int bytesPerPixel = image.infoHeader.biBitCount / 8;
    int rowSize = ((image.width() * bytesPerPixel + 3) / 4) * 4;

    file.seekg(image.fileHeader.bfOffBits, std::ios::beg);
    for (int y = 0; y < image.height(); ++y) {
        file.read(reinterpret_cast<char *>(image[y]), 3 * image.width());
        file.ignore(rowSize - image.width() * bytesPerPixel);
    }
    return image;
}
