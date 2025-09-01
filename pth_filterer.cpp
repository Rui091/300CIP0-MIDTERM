#include "ImageFactory.h"
#include "PGMImage.h"
#include "PPMImage.h"
#include "Filter.h"
#include "PthreadFilter.h"
#include <iostream>
#include <chrono>
#include <cstring>

void printUsage(const char* programName) {
    std::cout << "Uso: " << programName << " <archivo_entrada> <archivo_salida> --f <filtro>" << std::endl;
    std::cout << "Ejemplo: " << programName << " fruit.pgm fruit_blur2.pgm --f blur" << std::endl;
    std::cout << "\nFormatos soportados:" << std::endl;
    std::cout << "  - PPM (P3): Imágenes a color" << std::endl;
    std::cout << "  - PGM (P2): Imágenes en escala de grises" << std::endl;
    std::cout << "\nFiltros disponibles:" << std::endl;
    std::cout << "  - blur: Filtro de suavizado" << std::endl;
    std::cout << "  - laplace/laplacian: Filtro Laplaciano (detección de bordes)" << std::endl;
    std::cout << "  - sharpen/sharpening: Filtro de realce" << std::endl;
}

void measureAndApplyFilterPthread(const std::string& inputFilename, const std::string& outputFilename, const char* filterName) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Filtrador Paralelo con Pthreads" << std::endl;
    std::cout << "Procesando archivo: " << inputFilename << std::endl;
    std::cout << "Filtro: " << filterName << std::endl;
    std::cout << "Archivo de salida: " << outputFilename << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Medir tiempo de carga
    auto startLoad = std::chrono::high_resolution_clock::now();
    Image* image = ImageFactory::createImage(inputFilename);
    auto endLoad = std::chrono::high_resolution_clock::now();
    
    if (image == nullptr) {
        std::cerr << "Error: No se pudo cargar la imagen " << inputFilename << std::endl;
        return;
    }
    
    auto loadTime = std::chrono::duration_cast<std::chrono::microseconds>(endLoad - startLoad);
    std::cout << "Tiempo de carga: " << loadTime.count() << " microsegundos" << std::endl;
    
    // Mostrar información de la imagen
    image->displayInfo();
    
    // Crear filtro
    Filter* filter = FilterFactory::createFilter(filterName);
    if (filter == nullptr) {
        std::cerr << "Error: Filtro no reconocido: " << filterName << std::endl;
        delete image;
        return;
    }
    
    std::cout << "Aplicando filtro: " << filter->getName() << " con Pthreads (4 hilos)" << std::endl;
    
    // Medir tiempo de aplicación del filtro con pthreads
    auto startFilter = std::chrono::high_resolution_clock::now();
    Image* filteredImage = PthreadFilter::applyFilterParallel(image, filter);
    auto endFilter = std::chrono::high_resolution_clock::now();
    
    if (filteredImage == nullptr) {
        std::cerr << "Error: No se pudo aplicar el filtro con pthreads" << std::endl;
        delete filter;
        delete image;
        return;
    }
    
    auto filterTime = std::chrono::duration_cast<std::chrono::microseconds>(endFilter - startFilter);
    std::cout << "Tiempo de aplicación del filtro (Pthreads): " << filterTime.count() << " microsegundos" << std::endl;
    
    // Medir tiempo de guardado
    auto startSave = std::chrono::high_resolution_clock::now();
    bool success = filteredImage->writeToFile(outputFilename);
    auto endSave = std::chrono::high_resolution_clock::now();
    
    auto saveTime = std::chrono::duration_cast<std::chrono::microseconds>(endSave - startSave);
    std::cout << "Tiempo de guardado: " << saveTime.count() << " microsegundos" << std::endl;
    
    // Calcular tiempo total
    auto totalTime = loadTime + filterTime + saveTime;
    std::cout << "Tiempo total: " << totalTime.count() << " microsegundos" << std::endl;
    
    if (success) {
        std::cout << "Imagen filtrada guardada exitosamente en: " << outputFilename << std::endl;
    } else {
        std::cerr << "Error al guardar la imagen filtrada" << std::endl;
    }
    
    // Limpiar memoria
    delete filteredImage;
    delete filter;
    delete image;
    std::cout << "Procesamiento con Pthreads completado." << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Filtrador de Imágenes PPM/PGM - Versión Paralela con Pthreads ===" << std::endl;
    std::cout << "Programación Paralela - Parcial 1" << std::endl;
    
    if (argc != 5) {
        std::cerr << "Error: Número incorrecto de argumentos" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // Verificar que el tercer argumento sea --f
    if (strcmp(argv[3], "--f") != 0) {
        std::cerr << "Error: Se esperaba '--f' como tercer argumento" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];
    const char* filterName = argv[4];
    
    // Medir tiempo total de CPU y de pared
    auto cpuStartTime = std::clock();
    auto wallStartTime = std::chrono::high_resolution_clock::now();
    
    measureAndApplyFilterPthread(inputFilename, outputFilename, filterName);
    
    auto cpuEndTime = std::clock();
    auto wallEndTime = std::chrono::high_resolution_clock::now();
    
    // Calcular tiempos finales
    double cpuTime = static_cast<double>(cpuEndTime - cpuStartTime) / CLOCKS_PER_SEC * 1000000; // microsegundos
    auto wallTime = std::chrono::duration_cast<std::chrono::microseconds>(wallEndTime - wallStartTime);
    
    std::cout << "\n=== Resumen de Tiempos (Pthreads) ===" << std::endl;
    std::cout << "Tiempo de CPU: " << cpuTime << " microsegundos" << std::endl;
    std::cout << "Tiempo de ejecución total (wall-clock): " << wallTime.count() << " microsegundos" << std::endl;
    std::cout << "=== Filtrado con Pthreads finalizado ===" << std::endl;
    
    return 0;
}
