#include "image.h"
#include <fstream>
#include <iostream>
#include <cstring>

const double MAX_BRIGHT = 255;

inline int32_t Max(int32_t fr, int32_t sc) {
    return (fr > sc) ? (fr) : (sc);
}
inline int32_t Min(int32_t fr, int32_t sc) {
    return (fr < sc) ? (fr) : (sc);
}

Color::Color() {
    red = 0;
    green = 0;
    blue = 0;
}

Color::Color(double red, double green, double blue) {
    this->red = red;
    this->green = green;
    this->blue = blue;
}
Color operator+(const Color& fr, const Color& sc) {
    return Color(fr.red + sc.red, fr.green + sc.green, fr.blue + sc.blue);
}

Color operator-(const Color& fr) {
    return Color(-fr.red, -fr.green, -fr.blue);
}

Color& operator*=(Color& fr, double k) {
    fr.red *= static_cast<double>(k);
    fr.green *= static_cast<double>(k);
    fr.blue *= static_cast<double>(k);
    return fr;
}
Color operator*(const Color& fr, const double k) {
    Color result = fr;
    result *= k;
    return result;
}

Color& operator+=(Color& fr, const Color& sc) {
    fr.blue += sc.blue;
    fr.green += sc.green;
    fr.red += sc.red;
    return fr;
}

Color operator-(const Color& fr, const Color& sc) {
    return fr + (-sc);
}

void Image::Resize(const int32_t wid, const int32_t hei) {
    if (wid <= 0 || hei <= 0) {
        std::cout << "Too small crop sizes" << std::endl;
        std::exit(0);
    }
    if (array_ == nullptr) {
        width_ = wid;
        act_width_ = wid;
        height_ = hei;
        act_height_ = hei;
        array_ = new Color*[act_height_];
        for (size_t i = 0; i < act_height_; ++i) {
            array_[i] = new Color[act_width_];
        }
        return;
    }
    width_ = Min(static_cast<int32_t>(width_), wid);
    height_ = Min(static_cast<int32_t>(height_), hei);
}

Image::Image() {
    array_ = nullptr;
    header_ = Header();
    width_ = act_width_ = 0;
    height_ = act_height_ = 0;
}

Image::Image(std::string filename) {
    array_ = nullptr;
    width_ = act_width_ = 0;
    height_ = act_height_ = 0;

    try {
        std::ifstream input(filename, std::ios_base::binary);

        header_ = Header(input);
        Resize(static_cast<int32_t>(header_.GetWidth()), static_cast<int32_t>(header_.GetHeight()));

        char* buffer = new char[header_.ArraySize()];

        input.read(buffer, header_.ArraySize());
        input.close();
        // Image stored in such a way that array_[0][0] - top left corner
        // array_[height_ - 1][0] - left bottom
        size_t line_size = header_.ArraySize() / header_.GetHeight();
        for (size_t i = 0; i < height_; ++i) {
            for (size_t j = 0; j < width_; ++j) {
                array_[height_ - 1 - i][j] =
                    Color(static_cast<u_int8_t>(buffer[i * line_size + j * 3 + 2]) / MAX_BRIGHT,
                          static_cast<u_int8_t>(buffer[i * line_size + j * 3 + 1]) / MAX_BRIGHT,
                          static_cast<u_int8_t>(buffer[i * line_size + j * 3 + 0]) / MAX_BRIGHT);
            }
        }
        delete[] buffer;
    } catch (...) {
        std::cout << "Cant open " << filename << std::endl;
        std::exit(0);
    }
}

char ToChar(double val) {
    return static_cast<char>(static_cast<u_char>(Max(0, Min(MAX_BRIGHT, val * MAX_BRIGHT))));  // NOLINT
}

void Image::Save(const std::string filename) {
    try {
        std::ofstream output(filename, std::ios::binary);

        header_.SetSize(GetSize());

        char* buffer = new char[header_.HeaderSize() + header_.ArraySize()];
        memset(buffer, 0, header_.ArraySize() + header_.HeaderSize());
        memcpy(buffer, header_.BufferBegin(), header_.HeaderSize());
        size_t line_size = header_.ArraySize() / header_.GetHeight();
        for (size_t i = 0; i < height_; ++i) {
            for (size_t j = 0; j < width_; ++j) {
                size_t offset = header_.HeaderSize() + line_size * i + 3 * j;
                buffer[offset + 0] = ToChar(array_[height_ - 1 - i][j].blue);
                buffer[offset + 1] = ToChar(array_[height_ - 1 - i][j].green);
                buffer[offset + 2] = ToChar(array_[height_ - 1 - i][j].red);
            }
        }
        output.write(buffer, header_.ArraySize() + header_.HeaderSize());
        output.close();
        delete[] buffer;
    } catch (...) {
        std::cout << "Cant open output file\n";
        exit(0);
    }
}

void Image::Clear() {
    if (array_ != nullptr) {
        for (size_t i = 0; i < act_height_; ++i) {
            delete[] array_[i];
        }
        delete[] array_;
        array_ = nullptr;
    }
}

Image::~Image() {
    Clear();
}

void Image::operator=(const Image& oth) {
    width_ = act_width_ = oth.width_;
    height_ = act_height_ = oth.height_;
    header_ = oth.header_;
    if (oth.array_ == nullptr) {
        array_ = nullptr;
    } else {
        array_ = new Color*[height_];
        for (size_t i = 0; i < height_; ++i) {
            array_[i] = new Color[width_];
            memcpy(array_[i], oth.array_[i], width_ * sizeof(Color));
        }
    }
}
std::pair<size_t, size_t> Image::GetSize() {  // height, width
    return {height_, width_};
}

Color& Image::Get(int32_t h, int32_t w) {
    // принципиально нельзя использовать size_t, потому что он не поддерживает отрицательноые значения. для матричных
    // фильтров, которые выходят за границы изображения это оченб важно. А матрицы могут быть любых размеров, поэтому,
    // какой-то заранее указанный запас не спасет.
    if (array_ == nullptr) {
        throw "empty image, cant get pixel";
    }
    w = Max(0, Min(w, static_cast<int32_t>(width_) - 1));
    h = Max(0, Min(h, static_cast<int32_t>(height_) - 1));
    return array_[h][w];
}
