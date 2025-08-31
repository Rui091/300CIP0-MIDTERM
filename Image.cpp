#include "Image.h"
#include <sstream>

Image::Image() : width(0), height(0), maxVal(0), magicNumber("") {}

Image::Image(int w, int h, int max) : width(w), height(h), maxVal(max), magicNumber("") {}

void Image::displayInfo() const {
    std::cout << "Información de la imagen:" << std::endl;
    std::cout << "Número mágico: " << magicNumber << std::endl;
    std::cout << "Dimensiones: " << width << " x " << height << std::endl;
    std::cout << "Valor máximo: " << maxVal << std::endl;
}

void Image::skipComments(std::ifstream& file) const {
    char c;
    while (file.peek() == '#' || file.peek() == '\n' || file.peek() == '\r' || file.peek() == ' ' || file.peek() == '\t') {
        if (file.peek() == '#') {
            // Saltar toda la línea de comentario
            std::string line;
            std::getline(file, line);
        } else {
            // Saltar espacios en blanco y saltos de línea
            file.get(c);
        }
    }
}