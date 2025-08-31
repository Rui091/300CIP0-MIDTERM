#include "ImageFactory.h"
#include "PGMImage.h"
#include "PPMImage.h"
#include <iostream>
#include <vector>
#include <chrono>

void printUsage(const char* programName) {
    std::cout << "Uso: " << programName << " <archivo_entrada1> [archivo_entrada2] ..." << std::endl;
    std::cout << "Ejemplo: " << programName << " lena.ppm fruit.pgm" << std::endl;
    std::cout << "\nFormatos soportados:" << std::endl;
    std::cout << "  - PPM (P3): Imágenes a color" << std::endl;
    std::cout << "  - PGM (P2): Imágenes en escala de grises" << std::endl;
}

void processImage(const std::string& filename) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Procesando archivo: " << filename << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Determinar tipo de imagen
    std::string imageType = ImageFactory::getImageType(filename);
    std::cout << "Tipo de imagen detectado: " << imageType << std::endl;
    
    // Crear imagen usando el factory
    auto start = std::chrono::high_resolution_clock::now();
    Image* image = ImageFactory::createImage(filename);
    auto end = std::chrono::high_resolution_clock::now();
    
    if (image == nullptr) {
        std::cerr << "Error: No se pudo cargar la imagen " << filename << std::endl;
        return;
    }
    
    auto loadTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Tiempo de carga: " << loadTime.count() << " microsegundos" << std::endl;
    
    // Mostrar información de la imagen
    image->displayInfo();
    
    // Crear archivo de salida de prueba
    std::string outputFilename;
    if (imageType == "PPM") {
        outputFilename = "output_" + filename;
        std::cout << "Creando copia de prueba: " << outputFilename << std::endl;
        
        // Crear una copia y modificar algunos píxeles para prueba
        PPMImage* ppmImage = dynamic_cast<PPMImage*>(image);
        if (ppmImage) {
            PPMImage* copy = ppmImage->clone();
            
            // Modificar algunos píxeles como prueba (crear un pequeño cuadrado rojo en la esquina)
            int squareSize = std::min(10, std::min(copy->getWidth(), copy->getHeight()));
            for (int i = 0; i < squareSize; i++) {
                for (int j = 0; j < squareSize; j++) {
                    copy->setPixel(j, i, copy->getMaxVal(), 0, 0); // Rojo
                }
            }
            
            start = std::chrono::high_resolution_clock::now();
            bool success = copy->writeToFile(outputFilename);
            end = std::chrono::high_resolution_clock::now();
            
            auto saveTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "Tiempo de guardado: " << saveTime.count() << " microsegundos" << std::endl;
            
            if (success) {
                std::cout << "Archivo guardado exitosamente con marca de prueba (cuadrado rojo)" << std::endl;
            } else {
                std::cerr << "Error al guardar el archivo" << std::endl;
            }
            
            delete copy;
        }
    } else if (imageType == "PGM") {
        outputFilename = "output_" + filename;
        std::cout << "Creando copia de prueba: " << outputFilename << std::endl;
        
        // Crear una copia y modificar algunos píxeles para prueba
        PGMImage* pgmImage = dynamic_cast<PGMImage*>(image);
        if (pgmImage) {
            PGMImage* copy = pgmImage->clone();
            
            // Modificar algunos píxeles como prueba (crear un pequeño cuadrado blanco en la esquina)
            int squareSize = std::min(10, std::min(copy->getWidth(), copy->getHeight()));
            for (int i = 0; i < squareSize; i++) {
                for (int j = 0; j < squareSize; j++) {
                    copy->setPixel(j, i, copy->getMaxVal()); // Blanco
                }
            }
            
            start = std::chrono::high_resolution_clock::now();
            bool success = copy->writeToFile(outputFilename);
            end = std::chrono::high_resolution_clock::now();
            
            auto saveTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "Tiempo de guardado: " << saveTime.count() << " microsegundos" << std::endl;
            
            if (success) {
                std::cout << "Archivo guardado exitosamente con marca de prueba (cuadrado blanco)" << std::endl;
            } else {
                std::cerr << "Error al guardar el archivo" << std::endl;
            }
            
            delete copy;
        }
    }
    
    // Liberar memoria
    delete image;
    std::cout << "Procesamiento completado." << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Procesador de Imágenes PPM/PGM ===" << std::endl;
    std::cout << "Aplicación base - Programación Orientada a Objetos" << std::endl;
    
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Procesar cada archivo de entrada
    for (int i = 1; i < argc; i++) {
        processImage(argv[i]);
    }
    
    std::cout << "\n=== Procesamiento finalizado ===" << std::endl;
    return 0;
}