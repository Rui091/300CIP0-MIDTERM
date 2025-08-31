#ifndef IMAGEFACTORY_H
#define IMAGEFACTORY_H

#include "Image.h"
#include "PGMImage.h"
#include "PPMImage.h"
#include <string>

class ImageFactory {
public:
    // Método estático para crear imagen según el tipo
    static Image* createImage(const std::string& filename);
    
    // Método para determinar el tipo de imagen
    static std::string getImageType(const std::string& filename);
    
private:
    // Método auxiliar para leer el número mágico del archivo
    static std::string readMagicNumber(const std::string& filename);
};

#endif // IMAGEFACTORY_H