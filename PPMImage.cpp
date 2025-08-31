#include "PPMImage.h"
#include <sstream>

PPMImage::PPMImage() : Image(), pixels(nullptr) {}

PPMImage::PPMImage(int w, int h, int max) : Image(w, h, max), pixels(nullptr) {
    magicNumber = "P3";
    allocateMemory();
}

PPMImage::~PPMImage() {
    deallocateMemory();
}

PPMImage::PPMImage(const PPMImage& other) : Image(other.width, other.height, other.maxVal) {
    magicNumber = other.magicNumber;
    allocateMemory();
    copyPixels(other);
}

PPMImage& PPMImage::operator=(const PPMImage& other) {
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

bool PPMImage::readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
        return false;
    }
    
    // Leer número mágico
    file >> magicNumber;
    if (magicNumber != "P3") {
        std::cerr << "Error: Formato de archivo no válido. Se esperaba P3" << std::endl;
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
            int r, g, b;
            file >> r >> g >> b;
            if (file.fail()) {
                std::cerr << "Error: Error al leer píxel RGB en posición (" << i << ", " << j << ")" << std::endl;
                return false;
            }
            pixels[i][j] = RGB(r, g, b);
        }
    }
    
    file.close();
    return true;
}

bool PPMImage::writeToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo " << filename << std::endl;
        return false;
    }
    
    // Escribir encabezado
    file << magicNumber << std::endl;
    file << "# Generado por PPMImage" << std::endl;
    file << width << " " << height << std::endl;
    file << maxVal << std::endl;
    
    // Escribir píxeles
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            file << pixels[i][j].r << " " << pixels[i][j].g << " " << pixels[i][j].b;
            if (j < width - 1) file << "  ";
        }
        file << std::endl;
    }
    
    file.close();
    return true;
}

RGB PPMImage::getPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return pixels[y][x];
    }
    return RGB(0, 0, 0);
}

void PPMImage::setPixel(int x, int y, const RGB& color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        RGB clampedColor = color;
        // Asegurar que los valores estén en el rango válido
        if (clampedColor.r < 0) clampedColor.r = 0;
        if (clampedColor.r > maxVal) clampedColor.r = maxVal;
        if (clampedColor.g < 0) clampedColor.g = 0;
        if (clampedColor.g > maxVal) clampedColor.g = maxVal;
        if (clampedColor.b < 0) clampedColor.b = 0;
        if (clampedColor.b > maxVal) clampedColor.b = maxVal;
        
        pixels[y][x] = clampedColor;
    }
}

void PPMImage::setPixel(int x, int y, int r, int g, int b) {
    setPixel(x, y, RGB(r, g, b));
}

PPMImage* PPMImage::clone() const {
    return new PPMImage(*this);
}

void PPMImage::allocateMemory() {
    if (width > 0 && height > 0) {
        pixels = new RGB*[height];
        for (int i = 0; i < height; i++) {
            pixels[i] = new RGB[width];
            // Los píxeles se inicializan automáticamente con RGB(0,0,0)
        }
    }
}

void PPMImage::deallocateMemory() {
    if (pixels != nullptr) {
        for (int i = 0; i < height; i++) {
            delete[] pixels[i];
        }
        delete[] pixels;
        pixels = nullptr;
    }
}

void PPMImage::copyPixels(const PPMImage& other) {
    if (pixels != nullptr && other.pixels != nullptr) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                pixels[i][j] = other.pixels[i][j];
            }
        }
    }
}