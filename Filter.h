#ifndef FILTER_H
#define FILTER_H

#include "Image.h"
#include "PGMImage.h"
#include "PPMImage.h"

class Filter {
public:
    virtual ~Filter() = default;
    virtual Image* apply(const Image* input) = 0;
    virtual const char* getName() const = 0;
};

class BlurFilter : public Filter {
public:
    Image* apply(const Image* input) override;
    const char* getName() const override { return "Blur"; }
};

class LaplacianFilter : public Filter {
public:
    Image* apply(const Image* input) override;
    const char* getName() const override { return "Laplacian"; }
};

class SharpenFilter : public Filter {
public:
    Image* apply(const Image* input) override;
    const char* getName() const override { return "Sharpen"; }
};

// Factory para crear filtros
class FilterFactory {
public:
    static Filter* createFilter(const char* filterName);
};

#endif // FILTER_H
