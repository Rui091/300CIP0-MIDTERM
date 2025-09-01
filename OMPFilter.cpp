#include "OMPFilter.h"
#include "PGMImage.h"
#include "PPMImage.h"
#include <iostream>
#include <cmath>
#include <cstring>

Image* OMPFilter::applyFilterParallel(const Image* input, Filter* filter) {
    if (!input || !filter) return nullptr;
    
    // Crear imagen de salida
    Image* output = nullptr;
    const PGMImage* pgmInput = dynamic_cast<const PGMImage*>(input);
    const PPMImage* ppmInput = dynamic_cast<const PPMImage*>(input);
    
    if (pgmInput) {
        output = new PGMImage(input->getWidth(), input->getHeight(), input->getMaxVal());
    } else if (ppmInput) {
        output = new PPMImage(input->getWidth(), input->getHeight(), input->getMaxVal());
    } else {
        return nullptr;
    }
    
    // Determinar el kernel según el tipo de filtro
    double kernel[3][3];
    bool isLaplacian = false;
    
    if (strcmp(filter->getName(), "Blur") == 0) {
        double blurKernel[3][3] = {
            {1.0/9.0, 1.0/9.0, 1.0/9.0},
            {1.0/9.0, 1.0/9.0, 1.0/9.0},
            {1.0/9.0, 1.0/9.0, 1.0/9.0}
        };
        memcpy(kernel, blurKernel, sizeof(kernel));
    } else if (strcmp(filter->getName(), "Laplacian") == 0) {
        double laplacianKernel[3][3] = {
            { 0, -1,  0},
            {-1,  4, -1},
            { 0, -1,  0}
        };
        memcpy(kernel, laplacianKernel, sizeof(kernel));
        isLaplacian = true;
    } else if (strcmp(filter->getName(), "Sharpen") == 0) {
        double sharpenKernel[3][3] = {
            { 0, -1,  0},
            {-1,  5, -1},
            { 0, -1,  0}
        };
        memcpy(kernel, sharpenKernel, sizeof(kernel));
    }
    
    std::cout << "Aplicando filtro con OpenMP (hilos disponibles: " << omp_get_max_threads() << ")" << std::endl;
    
    // Aplicar filtro usando paralelización OpenMP
    applyKernelParallel(input, output, kernel, isLaplacian);
    
    std::cout << "Procesamiento paralelo con OpenMP completado" << std::endl;
    return output;
}

void OMPFilter::applyKernelParallel(const Image* input, Image* output, 
                                   double kernel[3][3], bool isLaplacian) {
    const PGMImage* pgmInput = dynamic_cast<const PGMImage*>(input);
    const PPMImage* ppmInput = dynamic_cast<const PPMImage*>(input);
    PGMImage* pgmOutput = dynamic_cast<PGMImage*>(output);
    PPMImage* ppmOutput = dynamic_cast<PPMImage*>(output);
    
    int width = input->getWidth();
    int height = input->getHeight();
    int maxVal = input->getMaxVal();
    
    if (pgmInput && pgmOutput) {
        // Procesar imagen PGM con OpenMP
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double sum = 0;
                
                // Aplicar kernel 3x3
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int px = x + kx;
                        int py = y + ky;
                        
                        // Manejo de bordes
                        if (px < 0) px = 0;
                        if (px >= width) px = width - 1;
                        if (py < 0) py = 0;
                        if (py >= height) py = height - 1;
                        
                        int pixel = pgmInput->getPixel(px, py);
                        sum += pixel * kernel[ky + 1][kx + 1];
                    }
                }
                
                int result = (int)std::round(sum);
                if (isLaplacian) {
                    result += 128; // Centrar resultado para Laplaciano
                }
                result = std::max(0, std::min(maxVal, result));
                pgmOutput->setPixel(x, y, result);
            }
        }
        
    } else if (ppmInput && ppmOutput) {
        // Procesar imagen PPM con OpenMP
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double sumR = 0, sumG = 0, sumB = 0;
                
                // Aplicar kernel 3x3
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int px = x + kx;
                        int py = y + ky;
                        
                        // Manejo de bordes
                        if (px < 0) px = 0;
                        if (px >= width) px = width - 1;
                        if (py < 0) py = 0;
                        if (py >= height) py = height - 1;
                        
                        RGB pixel = ppmInput->getPixel(px, py);
                        double kernelValue = kernel[ky + 1][kx + 1];
                        
                        sumR += pixel.r * kernelValue;
                        sumG += pixel.g * kernelValue;
                        sumB += pixel.b * kernelValue;
                    }
                }
                
                int r = (int)std::round(sumR);
                int g = (int)std::round(sumG);
                int b = (int)std::round(sumB);
                
                if (isLaplacian) {
                    r += 128;
                    g += 128;
                    b += 128;
                }
                
                r = std::max(0, std::min(maxVal, r));
                g = std::max(0, std::min(maxVal, g));
                b = std::max(0, std::min(maxVal, b));
                
                ppmOutput->setPixel(x, y, RGB(r, g, b));
            }
        }
    }
}
