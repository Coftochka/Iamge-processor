#include "header_tailer.h"
#include <cstring>
#include <iostream>

const size_t BMP_HEADER_SIZE = 14;
const size_t BYTE_IN_BITS = 8;
const size_t PIXEL_BEGIN_OFFSET = 10;
const size_t WIDTH_OFFSET = 18;
const size_t HEIGHT_OFFSET = 22;

Header::Header() {
    filesize_ = 0;
    buffer_ = nullptr;
    pixel_begin_ = 0;
    width_ = 0;
    height_ = 0;
}

u_int32_t ReadBytes(char* ptr) {
    u_int32_t value = 0;
    for (u_int32_t i = 0; i < 4; ++i) {
        value += (static_cast<u_int32_t>(static_cast<u_char>(ptr[i])) << (i * BYTE_IN_BITS));
    }
    return value;
}

void WriteBytes(u_int32_t value, char* ptr) {
    for (int i = 0; i < 4; ++i) {
        ptr[i] = static_cast<char>(static_cast<u_char>(value >> (i * BYTE_IN_BITS)));
    }
}

void Header::Clear() {
    if (buffer_ != nullptr) {
        delete[] buffer_;
        buffer_ = nullptr;
    }
}

Header::Header(std::ifstream& stream) {
    char* buffer0 = new char[BMP_HEADER_SIZE];
    stream.read(buffer0, BMP_HEADER_SIZE);
    pixel_begin_ = ReadBytes(buffer0 + PIXEL_BEGIN_OFFSET);
    filesize_ = ReadBytes(buffer0 + 2);

    buffer_ = new char[pixel_begin_];
    memcpy(buffer_, buffer0, BMP_HEADER_SIZE);
    delete[] buffer0;

    stream.read(buffer_ + BMP_HEADER_SIZE, pixel_begin_ - BMP_HEADER_SIZE);  // NOLINT

    width_ = ReadBytes(buffer_ + WIDTH_OFFSET);
    height_ = ReadBytes(buffer_ + HEIGHT_OFFSET);
}

void Header::SetSize(std::pair<size_t, size_t> size) {
    width_ = size.second;
    height_ = size.first;

    u_int32_t line_byte_size = ((width_ * 3 + 3) / 4) * 4;
    filesize_ = line_byte_size * height_ + pixel_begin_;

    WriteBytes(width_, buffer_ + WIDTH_OFFSET);
    WriteBytes(height_, buffer_ + HEIGHT_OFFSET);
    WriteBytes(filesize_, buffer_ + 2);
}

Header& Header::operator=(const Header& h2) {
    Clear();
    if (h2.BufferBegin() != nullptr) {
        buffer_ = new char[h2.HeaderSize()];
        memcpy(buffer_, h2.BufferBegin(), h2.pixel_begin_);
    } else {
        buffer_ = nullptr;
    }
    filesize_ = h2.filesize_;
    pixel_begin_ = h2.pixel_begin_;
    width_ = h2.width_;
    height_ = h2.height_;
    return *this;
}

Header::~Header() {
    Clear();
}
u_int32_t Header::GetWidth() const {
    return width_;
}
u_int32_t Header::GetHeight() const {
    return height_;
}
u_int32_t Header::ArraySize() const {
    return filesize_ - pixel_begin_;
}
const char* Header::BufferBegin() const {
    return buffer_;
}
u_int32_t Header::HeaderSize() const {
    return pixel_begin_;
}
