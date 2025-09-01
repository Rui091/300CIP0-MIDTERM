#include "PthreadFilter.h"
#include "PGMImage.h"
#include "PPMImage.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>

Image* PthreadFilter::applyFilterParallel(const Image* input, Filter* filter) {
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
    
    // Configurar 4 hilos para 4 regiones
    const int NUM_THREADS = 4;
    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    
    int width = input->getWidth();
    int height = input->getHeight();
    int halfWidth = width / 2;
    int halfHeight = height / 2;
    
    // Configurar datos para cada hilo
    // Región 1: Arriba-izquierda
    threadData[0] = {input, output, filter, 0, halfWidth, 0, halfHeight, 0};
    
    // Región 2: Arriba-derecha
    threadData[1] = {input, output, filter, halfWidth, width, 0, halfHeight, 1};
    
    // Región 3: Abajo-izquierda
    threadData[2] = {input, output, filter, 0, halfWidth, halfHeight, height, 2};
    
    // Región 4: Abajo-derecha
    threadData[3] = {input, output, filter, halfWidth, width, halfHeight, height, 3};
    
    std::cout << "Dividiendo imagen en 4 regiones para procesamiento paralelo:" << std::endl;
    std::cout << "  Región 1 (Arriba-izquierda): (" << 0 << "," << 0 << ") a (" << halfWidth << "," << halfHeight << ")" << std::endl;
    std::cout << "  Región 2 (Arriba-derecha): (" << halfWidth << "," << 0 << ") a (" << width << "," << halfHeight << ")" << std::endl;
    std::cout << "  Región 3 (Abajo-izquierda): (" << 0 << "," << halfHeight << ") a (" << halfWidth << "," << height << ")" << std::endl;
    std::cout << "  Región 4 (Abajo-derecha): (" << halfWidth << "," << halfHeight << ") a (" << width << "," << height << ")" << std::endl;
    
    // Crear hilos
    for (int i = 0; i < NUM_THREADS; i++) {
        int result = pthread_create(&threads[i], nullptr, threadFunction, &threadData[i]);
        if (result != 0) {
            std::cerr << "Error al crear hilo " << i << std::endl;
            delete output;
            return nullptr;
        }
    }
    
    // Esperar a que terminen todos los hilos
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], nullptr);
    }
    
    std::cout << "Procesamiento paralelo completado con " << NUM_THREADS << " hilos" << std::endl;
    return output;
}

void* PthreadFilter::threadFunction(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    
    std::cout << "Hilo " << data->threadId << " procesando región (" 
              << data->startX << "," << data->startY << ") a (" 
              << data->endX << "," << data->endY << ")" << std::endl;
    
    applyFilterToRegion(data->inputImage, data->outputImage, data->filter,
                       data->startX, data->endX, data->startY, data->endY);
    
    std::cout << "Hilo " << data->threadId << " completado" << std::endl;
    return nullptr;
}

void PthreadFilter::applyFilterToRegion(const Image* input, Image* output, Filter* filter,
                                      int startX, int endX, int startY, int endY) {
    const PGMImage* pgmInput = dynamic_cast<const PGMImage*>(input);
    const PPMImage* ppmInput = dynamic_cast<const PPMImage*>(input);
    PGMImage* pgmOutput = dynamic_cast<PGMImage*>(output);
    PPMImage* ppmOutput = dynamic_cast<PPMImage*>(output);
    
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
    
    // Aplicar filtro a la región asignada
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            if (pgmInput && pgmOutput) {
                // Procesar imagen PGM
                double sum = 0;
                
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int px = x + kx;
                        int py = y + ky;
                        
                        // Manejo de bordes
                        if (px < 0) px = 0;
                        if (px >= input->getWidth()) px = input->getWidth() - 1;
                        if (py < 0) py = 0;
                        if (py >= input->getHeight()) py = input->getHeight() - 1;
                        
                        int pixel = pgmInput->getPixel(px, py);
                        sum += pixel * kernel[ky + 1][kx + 1];
                    }
                }
                
                int result = (int)std::round(sum);
                if (isLaplacian) {
                    result += 128; // Centrar resultado para Laplaciano
                }
                result = std::max(0, std::min(input->getMaxVal(), result));
                pgmOutput->setPixel(x, y, result);
                
            } else if (ppmInput && ppmOutput) {
                // Procesar imagen PPM
                double sumR = 0, sumG = 0, sumB = 0;
                
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int px = x + kx;
                        int py = y + ky;
                        
                        // Manejo de bordes
                        if (px < 0) px = 0;
                        if (px >= input->getWidth()) px = input->getWidth() - 1;
                        if (py < 0) py = 0;
                        if (py >= input->getHeight()) py = input->getHeight() - 1;
                        
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
                
                r = std::max(0, std::min(input->getMaxVal(), r));
                g = std::max(0, std::min(input->getMaxVal(), g));
                b = std::max(0, std::min(input->getMaxVal(), b));
                
                ppmOutput->setPixel(x, y, RGB(r, g, b));
            }
        }
    }
}
