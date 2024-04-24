#include <cstdlib>
#include <iostream>
#include "header_tailer.h"

struct Color {
    double red;
    double green;
    double blue;
    Color();
    Color(double red, double green, double blue);
    friend Color operator+(const Color& fr, const Color& sc);
    friend Color operator-(const Color& fr);
    friend Color operator*(const Color& fr, double k);
    friend Color& operator+=(Color& fr, const Color& sc);
    friend Color& operator*=(Color& fr, double k);
    friend Color operator-(const Color& fr, const Color& sc);
};

class Image {

public:
    void Load(std::string filename);
    void Clear();
    void Resize(const int32_t wid, const int32_t hei);
    Color& Get(int32_t height, int32_t width);
    Image();
    explicit Image(std::string filename);
    Image(size_t height, size_t width);

    void operator=(const Image& oth);

    ~Image();
    std::pair<size_t, size_t> GetSize();  // height, width
    void Save(const std::string filename);

private:
    size_t width_;
    size_t height_;  // sizes of image
    size_t act_width_;
    size_t act_height_;  // actual sizes of array (to aviod memory
    // reallocation when cropping)
    Color** array_;
    Header header_;
};
