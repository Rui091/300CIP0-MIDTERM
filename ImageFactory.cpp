#include "ImageFactory.h"
#include <fstream>

Image* ImageFactory::createImage(const std::string& filename) {
    std::string magicNumber = readMagicNumber(filename);
    
    if (magicNumber == "P2") {
        PGMImage* image = new PGMImage();
        if (image->readFromFile(filename)) {
            return image;
        } else {
            delete image;
            return nullptr;
        }
    } else if (magicNumber == "P3") {
        PPMImage* image = new PPMImage();
        if (image->readFromFile(filename)) {
            return image;
        } else {
            delete image;
            return nullptr;
        }
    } else {
        std::cerr << "Error: Formato de archivo no soportado. Número mágico: " << magicNumber << std::endl;
        return nullptr;
    }
}

std::string ImageFactory::getImageType(const std::string& filename) {
    std::string magicNumber = readMagicNumber(filename);
    
    if (magicNumber == "P2") {
        return "PGM";
    } else if (magicNumber == "P3") {
        return "PPM";
    } else {
        return "UNKNOWN";
    }
}

std::string ImageFactory::readMagicNumber(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
        return "";
    }
    
    std::string magicNumber;
    file >> magicNumber;
    file.close();
    
    return magicNumber;
}