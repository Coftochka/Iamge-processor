#include "image.h"

class Filter {
public:
    void virtual Process(Image& image);
};

class GrayScale : public Filter {
public:
    void Process(Image& image) override;
};

class Inversion : public Filter {
public:
    void Process(Image& image) override;
};

class Crop : public Filter {
public:
    void Process(Image& image) override;
    Crop();
    Crop(u_int32_t height, u_int32_t width);

private:
    u_int32_t width_;
    u_int32_t height_;
};

class MatrixFilter : public Filter {
public:
    void Process(Image& img) override;

    MatrixFilter();

protected:
    double** matrix;
    int radius;  // ####---; radius = 3;
};

class Sharpening : public MatrixFilter {
public:
    Sharpening();
    ~Sharpening();
};

class EdgeDetection : public MatrixFilter, public GrayScale {
public:
    void Process(Image& img) override;
    explicit EdgeDetection(double thrashold);
    ~EdgeDetection();

public:
    double thrashold;
};