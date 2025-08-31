#include "PGMImage.h"
#include <sstream>

PGMImage::PGMImage() : Image(), pixels(nullptr) {}

PGMImage::PGMImage(int w, int h, int max) : Image(w, h, max), pixels(nullptr) {
    magicNumber = "P2";
    allocateMemory();
}

PGMImage::~PGMImage() {
    deallocateMemory();
}

PGMImage::PGMImage(const PGMImage& other) : Image(other.width, other.height, other.maxVal) {
    magicNumber = other.magicNumber;
    allocateMemory();
    copyPixels(other);
}

PGMImage& PGMImage::operator=(const PGMImage& other) {
    if (this != &other) {
        deallocateMemory();
        width = other.width;
        height = other.height;
        maxVal = other.maxVal;
        magicNumber = other.magicNumber;
        allocateMemory();
        copyPixels(other);
    }
    return *this;
}

bool PGMImage::readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
        return false;
    }
    
    // Leer número mágico
    file >> magicNumber;
    if (magicNumber != "P2") {
        std::cerr << "Error: Formato de archivo no válido. Se esperaba P2" << std::endl;
        return false;
    }
    
    // Saltar comentarios
    skipComments(file);
    
    // Leer dimensiones
    file >> width >> height;
    
    // Validar dimensiones
    if (width <= 0 || height <= 0) {
        std::cerr << "Error: Dimensiones inválidas: " << width << " x " << height << std::endl;
        return false;
    }
    
    // Saltar comentarios
    skipComments(file);
    
    // Leer valor máximo
    file >> maxVal;
    
    // Validar valor máximo
    if (maxVal <= 0) {
        std::cerr << "Error: Valor máximo inválido: " << maxVal << std::endl;
        return false;
    }
    
    // Allocar memoria
    deallocateMemory();
    allocateMemory();
    
    // Leer píxeles
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            file >> pixels[i][j];
            if (file.fail()) {
                std::cerr << "Error: Error al leer píxel en posición (" << i << ", " << j << ")" << std::endl;
                return false;
            }
        }
    }
    
    file.close();
    return true;
}

bool PGMImage::writeToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo " << filename << std::endl;
        return false;
    }
    
    // Escribir encabezado
    file << magicNumber << std::endl;
    file << "# Generado por PGMImage" << std::endl;
    file << width << " " << height << std::endl;
    file << maxVal << std::endl;
    
    // Escribir píxeles
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            file << pixels[i][j];
            if (j < width - 1) file << " ";
        }
        file << std::endl;
    }
    
    file.close();
    return true;
}

int PGMImage::getPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return pixels[y][x];
    }
    return 0;
}

void PGMImage::setPixel(int x, int y, int value) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        // Asegurar que el valor esté en el rango válido
        if (value < 0) value = 0;
        if (value > maxVal) value = maxVal;
        pixels[y][x] = value;
    }
}

PGMImage* PGMImage::clone() const {
    return new PGMImage(*this);
}

void PGMImage::allocateMemory() {
    if (width > 0 && height > 0) {
        pixels = new int*[height];
        for (int i = 0; i < height; i++) {
            pixels[i] = new int[width];
            // Inicializar con 0
            for (int j = 0; j < width; j++) {
                pixels[i][j] = 0;
            }
        }
    }
}

void PGMImage::deallocateMemory() {
    if (pixels != nullptr) {
        for (int i = 0; i < height; i++) {
            delete[] pixels[i];
        }
        delete[] pixels;
        pixels = nullptr;
    }
}

void PGMImage::copyPixels(const PGMImage& other) {
    if (pixels != nullptr && other.pixels != nullptr) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                pixels[i][j] = other.pixels[i][j];
            }
        }
    }
}