#include "Filter.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <cmath>

// Función auxiliar para aplicar convolución con kernel 3x3 para PGM
int applyKernel3x3PGM(const PGMImage* image, int x, int y, double kernel[3][3]) {
    double sum = 0;
    
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            int px = x + kx;
            int py = y + ky;
            
            // Manejo de bordes - replicar píxeles del borde
            if (px < 0) px = 0;
            if (px >= image->getWidth()) px = image->getWidth() - 1;
            if (py < 0) py = 0;
            if (py >= image->getHeight()) py = image->getHeight() - 1;
            
            int pixel = image->getPixel(px, py);
            double kernelValue = kernel[ky + 1][kx + 1];
            sum += pixel * kernelValue;
        }
    }
    
    return (int)std::round(sum);
}

// Función auxiliar para aplicar convolución con kernel 3x3 para PPM
RGB applyKernel3x3PPM(const PPMImage* image, int x, int y, double kernel[3][3]) {
    double sumR = 0, sumG = 0, sumB = 0;
    
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            int px = x + kx;
            int py = y + ky;
            
            // Manejo de bordes - replicar píxeles del borde
            if (px < 0) px = 0;
            if (px >= image->getWidth()) px = image->getWidth() - 1;
            if (py < 0) py = 0;
            if (py >= image->getHeight()) py = image->getHeight() - 1;
            
            RGB pixel = image->getPixel(px, py);
            double kernelValue = kernel[ky + 1][kx + 1];
            
            sumR += pixel.r * kernelValue;
            sumG += pixel.g * kernelValue;
            sumB += pixel.b * kernelValue;
        }
    }
    
    return RGB((int)std::round(sumR), (int)std::round(sumG), (int)std::round(sumB));
}

// Implementación BlurFilter
Image* BlurFilter::apply(const Image* input) {
    if (!input) return nullptr;
    
    // Kernel de suavizado (blur) 3x3
    double blurKernel[3][3] = {
        {1.0/9.0, 1.0/9.0, 1.0/9.0},
        {1.0/9.0, 1.0/9.0, 1.0/9.0},
        {1.0/9.0, 1.0/9.0, 1.0/9.0}
    };
    
    const PGMImage* pgmInput = dynamic_cast<const PGMImage*>(input);
    const PPMImage* ppmInput = dynamic_cast<const PPMImage*>(input);
    
    if (pgmInput) {
        PGMImage* output = new PGMImage(input->getWidth(), input->getHeight(), input->getMaxVal());
        
        for (int y = 0; y < input->getHeight(); y++) {
            for (int x = 0; x < input->getWidth(); x++) {
                int result = applyKernel3x3PGM(pgmInput, x, y, blurKernel);
                result = std::max(0, std::min(input->getMaxVal(), result));
                output->setPixel(x, y, result);
            }
        }
        return output;
    } 
    else if (ppmInput) {
        PPMImage* output = new PPMImage(input->getWidth(), input->getHeight(), input->getMaxVal());
        
        for (int y = 0; y < input->getHeight(); y++) {
            for (int x = 0; x < input->getWidth(); x++) {
                RGB result = applyKernel3x3PPM(ppmInput, x, y, blurKernel);
                result.r = std::max(0, std::min(input->getMaxVal(), result.r));
                result.g = std::max(0, std::min(input->getMaxVal(), result.g));
                result.b = std::max(0, std::min(input->getMaxVal(), result.b));
                output->setPixel(x, y, result);
            }
        }
        return output;
    }
    
    return nullptr;
}

// Implementación LaplacianFilter
Image* LaplacianFilter::apply(const Image* input) {
    if (!input) return nullptr;
    
    // Kernel Laplaciano 3x3
    double laplacianKernel[3][3] = {
        { 0, -1,  0},
        {-1,  4, -1},
        { 0, -1,  0}
    };
    
    const PGMImage* pgmInput = dynamic_cast<const PGMImage*>(input);
    const PPMImage* ppmInput = dynamic_cast<const PPMImage*>(input);
    
    if (pgmInput) {
        PGMImage* output = new PGMImage(input->getWidth(), input->getHeight(), input->getMaxVal());
        
        for (int y = 0; y < input->getHeight(); y++) {
            for (int x = 0; x < input->getWidth(); x++) {
                int result = applyKernel3x3PGM(pgmInput, x, y, laplacianKernel);
                // Para Laplaciano, agregar 128 para centrar el resultado
                result = result + 128;
                result = std::max(0, std::min(input->getMaxVal(), result));
                output->setPixel(x, y, result);
            }
        }
        return output;
    }
    else if (ppmInput) {
        PPMImage* output = new PPMImage(input->getWidth(), input->getHeight(), input->getMaxVal());
        
        for (int y = 0; y < input->getHeight(); y++) {
            for (int x = 0; x < input->getWidth(); x++) {
                RGB result = applyKernel3x3PPM(ppmInput, x, y, laplacianKernel);
                // Para Laplaciano, agregar 128 para centrar el resultado
                result.r = std::max(0, std::min(input->getMaxVal(), result.r + 128));
                result.g = std::max(0, std::min(input->getMaxVal(), result.g + 128));
                result.b = std::max(0, std::min(input->getMaxVal(), result.b + 128));
                output->setPixel(x, y, result);
            }
        }
        return output;
    }
    
    return nullptr;
}

// Implementación SharpenFilter
Image* SharpenFilter::apply(const Image* input) {
    if (!input) return nullptr;
    
    // Kernel de realce (sharpening) 3x3
    double sharpenKernel[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    
    const PGMImage* pgmInput = dynamic_cast<const PGMImage*>(input);
    const PPMImage* ppmInput = dynamic_cast<const PPMImage*>(input);
    
    if (pgmInput) {
        PGMImage* output = new PGMImage(input->getWidth(), input->getHeight(), input->getMaxVal());
        
        for (int y = 0; y < input->getHeight(); y++) {
            for (int x = 0; x < input->getWidth(); x++) {
                int result = applyKernel3x3PGM(pgmInput, x, y, sharpenKernel);
                result = std::max(0, std::min(input->getMaxVal(), result));
                output->setPixel(x, y, result);
            }
        }
        return output;
    }
    else if (ppmInput) {
        PPMImage* output = new PPMImage(input->getWidth(), input->getHeight(), input->getMaxVal());
        
        for (int y = 0; y < input->getHeight(); y++) {
            for (int x = 0; x < input->getWidth(); x++) {
                RGB result = applyKernel3x3PPM(ppmInput, x, y, sharpenKernel);
                result.r = std::max(0, std::min(input->getMaxVal(), result.r));
                result.g = std::max(0, std::min(input->getMaxVal(), result.g));
                result.b = std::max(0, std::min(input->getMaxVal(), result.b));
                output->setPixel(x, y, result);
            }
        }
        return output;
    }
    
    return nullptr;
}

// Implementación FilterFactory
Filter* FilterFactory::createFilter(const char* filterName) {
    if (strcmp(filterName, "blur") == 0) {
        return new BlurFilter();
    } else if (strcmp(filterName, "laplace") == 0 || strcmp(filterName, "laplacian") == 0) {
        return new LaplacianFilter();
    } else if (strcmp(filterName, "sharpen") == 0 || strcmp(filterName, "sharpening") == 0) {
        return new SharpenFilter();
    }
    return nullptr;
}
