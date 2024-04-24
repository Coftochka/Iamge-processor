#include "filters.h"
const double RED_COEF = 0.299;
const double GREEN_COEF = 0.587;
const double BLUE_COEF = 0.114;

void Filter::Process(Image& img) {
    ;
}
void GrayScale::Process(Image& img) {
    size_t height = img.GetSize().first;
    size_t width = img.GetSize().second;
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            Color& pixel = img.Get(static_cast<int32_t>(i), static_cast<int32_t>(j));
            double value = RED_COEF * pixel.red + GREEN_COEF * pixel.green + BLUE_COEF * pixel.blue;
            pixel = Color(value, value, value);
        }
    }
}
void Inversion::Process(Image& img) {
    size_t height = img.GetSize().first;
    size_t width = img.GetSize().second;
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            Color& pixel = img.Get(static_cast<int32_t>(i), static_cast<int32_t>(j));
            pixel = Color(1.0, 1.0, 1.0) - pixel;
        }
    }
}

void Crop::Process(Image& image) {
    image.Resize(static_cast<int32_t>(height_), static_cast<int32_t>(width_));
}
Crop::Crop() {
    width_ = height_ = 0;
}

Crop::Crop(u_int32_t height, u_int32_t width) {
    height_ = height;
    width_ = width;
}

MatrixFilter::MatrixFilter() {
    matrix = nullptr;
    radius = 0;
}

void MatrixFilter::Process(Image& img) {
    if (matrix == nullptr) {
        throw "fyou";
    }
    Image buffer;
    buffer = img;
    size_t width = img.GetSize().second;
    size_t height = img.GetSize().first;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            Color& target = img.Get(i, j);
            target = Color(0.0, 0.0, 0.0);
            for (int di = -radius; di <= radius; ++di) {
                for (int dj = -radius; dj <= radius; ++dj) {
                    Color add = buffer.Get(i + di, j + dj) * (matrix[radius + di][radius + dj]);
                    target += add;
                }
            }
        }
    }
}

Sharpening::Sharpening() {
    matrix = new double*[3];
    radius = 1;
    for (int i = 0; i < 3; ++i) {
        matrix[i] = new double[3];
    }
    matrix[0][0] = matrix[2][2] = matrix[2][0] = matrix[0][2] = 0;
    matrix[1][0] = matrix[0][1] = matrix[2][1] = matrix[1][2] = -1;
    matrix[1][1] = 5;  // NOLINT
}

Sharpening::~Sharpening() {
    for (int i = 0; i < 3; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

EdgeDetection::EdgeDetection(double thrashold) {
    matrix = new double*[3];
    radius = 1;
    this->thrashold = thrashold;
    for (int i = 0; i < 3; ++i) {
        matrix[i] = new double[3];
    }
    matrix[0][0] = matrix[2][2] = matrix[2][0] = matrix[0][2] = 0;
    matrix[1][0] = matrix[0][1] = matrix[2][1] = matrix[1][2] = -1;
    matrix[1][1] = 4;
}

void EdgeDetection::Process(Image& img) {
    GrayScale::Process(img);
    MatrixFilter::Process(img);

    size_t width = img.GetSize().second;
    size_t height = img.GetSize().first;
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            Color& pixel = img.Get(static_cast<int32_t>(i), static_cast<int32_t>(j));
            pixel = (pixel.red >= thrashold) ? (Color(1.0, 1.0, 1.0)) : (Color(0.0, 0.0, 0.0));
        }
    }
}

EdgeDetection::~EdgeDetection() {
    for (int i = 0; i < 3; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}
