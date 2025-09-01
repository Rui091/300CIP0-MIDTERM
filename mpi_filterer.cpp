#include "ImageFactory.h"
#include "PGMImage.h"
#include "PPMImage.h"
#include "Filter.h"
#include "MPIFilter.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <mpi.h>

void printUsage(const char* programName) {
    std::cout << "Uso: mpirun -np <num_procesos> " << programName << " <archivo_entrada> <archivo_salida> --f <filtro>" << std::endl;
    std::cout << "Ejemplo: mpirun -np 4 " << programName << " sulfur.pgm sulfur_mpi.pgm --f blur" << std::endl;
    std::cout << "\nFormatos soportados:" << std::endl;
    std::cout << "  - PPM (P3): Imágenes a color" << std::endl;
    std::cout << "  - PGM (P2): Imágenes en escala de grises" << std::endl;
    std::cout << "\nFiltros disponibles:" << std::endl;
    std::cout << "  - blur: Filtro de suavizado" << std::endl;
    std::cout << "  - laplace/laplacian: Filtro Laplaciano (detección de bordes)" << std::endl;
    std::cout << "  - sharpen/sharpening: Filtro de realce" << std::endl;
}

void measureAndApplyFilterMPI(const std::string& inputFilename, const std::string& outputFilename, 
                             const char* filterName, int rank, int size) {
    if (rank == 0) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Filtrador Distribuido con MPI" << std::endl;
        std::cout << "Procesando archivo: " << inputFilename << std::endl;
        std::cout << "Filtro: " << filterName << std::endl;
        std::cout << "Archivo de salida: " << outputFilename << std::endl;
        std::cout << "Procesos MPI: " << size << std::endl;
        std::cout << "========================================" << std::endl;
    }
    
    Image* image = nullptr;
    auto loadTime = std::chrono::microseconds(0);
    
    // Solo el proceso 0 carga la imagen
    if (rank == 0) {
        auto startLoad = std::chrono::high_resolution_clock::now();
        image = ImageFactory::createImage(inputFilename);
        auto endLoad = std::chrono::high_resolution_clock::now();
        
        if (image == nullptr) {
            std::cerr << "Error: No se pudo cargar la imagen " << inputFilename << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        loadTime = std::chrono::duration_cast<std::chrono::microseconds>(endLoad - startLoad);
        std::cout << "Tiempo de carga: " << loadTime.count() << " microsegundos" << std::endl;
        
        // Mostrar información de la imagen
        image->displayInfo();
    }
    
    // Broadcast información básica de la imagen a todos los procesos
    int width, height, maxVal;
    char magicNumberArray[10];
    
    if (rank == 0) {
        width = image->getWidth();
        height = image->getHeight();
        maxVal = image->getMaxVal();
        strncpy(magicNumberArray, image->getMagicNumber().c_str(), sizeof(magicNumberArray));
    }
    
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&maxVal, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(magicNumberArray, sizeof(magicNumberArray), MPI_CHAR, 0, MPI_COMM_WORLD);
    
    // Los demás procesos crean una imagen vacía con las dimensiones correctas
    if (rank != 0) {
        std::string magicNumber(magicNumberArray);
        if (magicNumber == "P2") {
            image = new PGMImage(width, height, maxVal);
        } else if (magicNumber == "P3") {
            image = new PPMImage(width, height, maxVal);
        }
        
        if (image == nullptr) {
            std::cerr << "Proceso " << rank << ": Error creando imagen" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    
    // TODO: En una implementación completa, aquí se distribuirían los datos de píxeles
    // Por simplicidad, asumimos que todos los procesos tienen acceso a la imagen completa
    
    // Crear filtro
    Filter* filter = FilterFactory::createFilter(filterName);
    if (filter == nullptr) {
        if (rank == 0) {
            std::cerr << "Error: Filtro no reconocido: " << filterName << std::endl;
        }
        delete image;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    if (rank == 0) {
        std::cout << "Aplicando filtro: " << filter->getName() << " con MPI" << std::endl;
    }
    
    // Medir tiempo de aplicación del filtro con MPI
    MPI_Barrier(MPI_COMM_WORLD);
    auto startFilter = std::chrono::high_resolution_clock::now();
    
    Image* filteredImage = MPIFilter::applyFilterDistributed(image, filter, rank, size);
    
    MPI_Barrier(MPI_COMM_WORLD);
    auto endFilter = std::chrono::high_resolution_clock::now();
    
    if (filteredImage == nullptr) {
        if (rank == 0) {
            std::cerr << "Error: No se pudo aplicar el filtro con MPI" << std::endl;
        }
        delete filter;
        delete image;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    auto filterTime = std::chrono::duration_cast<std::chrono::microseconds>(endFilter - startFilter);
    
    if (rank == 0) {
        std::cout << "Tiempo de aplicación del filtro (MPI): " << filterTime.count() << " microsegundos" << std::endl;
    }
    
    // Recopilar resultados de todos los procesos
    Image* completeImage = MPIFilter::gatherImage(filteredImage, image->getWidth(), 
                                                 image->getHeight(), image->getMaxVal(),
                                                 image->getMagicNumber(), rank, size);
    
    // Solo el proceso 0 guarda el resultado
    auto saveTime = std::chrono::microseconds(0);
    if (rank == 0) {
        auto startSave = std::chrono::high_resolution_clock::now();
        bool success = completeImage->writeToFile(outputFilename);
        auto endSave = std::chrono::high_resolution_clock::now();
        
        saveTime = std::chrono::duration_cast<std::chrono::microseconds>(endSave - startSave);
        std::cout << "Tiempo de guardado: " << saveTime.count() << " microsegundos" << std::endl;
        
        // Calcular tiempo total
        auto totalTime = loadTime + filterTime + saveTime;
        std::cout << "Tiempo total: " << totalTime.count() << " microsegundos" << std::endl;
        
        if (success) {
            std::cout << "Imagen filtrada guardada exitosamente en: " << outputFilename << std::endl;
        } else {
            std::cerr << "Error al guardar la imagen filtrada" << std::endl;
        }
    }
    
    // Limpiar memoria
    delete filteredImage;
    if (rank == 0 && completeImage) {
        delete completeImage;
    }
    delete filter;
    delete image;
    
    if (rank == 0) {
        std::cout << "Procesamiento con MPI completado." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        std::cout << "=== Filtrador de Imágenes PPM/PGM - Versión Distribuida con MPI ===" << std::endl;
        std::cout << "Programación Paralela - Parcial 1" << std::endl;
    }
    
    if (argc != 5) {
        if (rank == 0) {
            std::cerr << "Error: Número incorrecto de argumentos" << std::endl;
            printUsage(argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    // Verificar que el tercer argumento sea --f
    if (strcmp(argv[3], "--f") != 0) {
        if (rank == 0) {
            std::cerr << "Error: Se esperaba '--f' como tercer argumento" << std::endl;
            printUsage(argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];
    const char* filterName = argv[4];
    
    // Medir tiempo total de CPU y de pared
    auto cpuStartTime = std::clock();
    auto wallStartTime = std::chrono::high_resolution_clock::now();
    
    measureAndApplyFilterMPI(inputFilename, outputFilename, filterName, rank, size);
    
    auto cpuEndTime = std::clock();
    auto wallEndTime = std::chrono::high_resolution_clock::now();
    
    // Calcular tiempos finales
    double cpuTime = static_cast<double>(cpuEndTime - cpuStartTime) / CLOCKS_PER_SEC * 1000000; // microsegundos
    auto wallTime = std::chrono::duration_cast<std::chrono::microseconds>(wallEndTime - wallStartTime);
    
    if (rank == 0) {
        std::cout << "\n=== Resumen de Tiempos (MPI) ===" << std::endl;
        std::cout << "Tiempo de CPU (proceso 0): " << cpuTime << " microsegundos" << std::endl;
        std::cout << "Tiempo de ejecución total (wall-clock): " << wallTime.count() << " microsegundos" << std::endl;
        std::cout << "=== Filtrado con MPI finalizado ===" << std::endl;
    }
    
    MPI_Finalize();
    return 0;
}
