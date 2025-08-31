#ifndef PPMIMAGE_H
#define PPMIMAGE_H

#include "Image.h"

struct RGB {
    int r, g, b;
    
    RGB() : r(0), g(0), b(0) {}
    RGB(int red, int green, int blue) : r(red), g(green), b(blue) {}
};

class PPMImage : public Image {
private:
    RGB** pixels;  // Matriz de píxeles RGB
    
public:
    // Constructor
    PPMImage();
    PPMImage(int w, int h, int max);
    
    // Destructor
    ~PPMImage();
    
    // Constructor de copia
    PPMImage(const PPMImage& other);
    
    // Operador de asignación
    PPMImage& operator=(const PPMImage& other);
    
    // Implementación de métodos virtuales
    bool readFromFile(const std::string& filename) override;
    bool writeToFile(const std::string& filename) const override;
    
    // Métodos específicos de PPM
    RGB getPixel(int x, int y) const;
    void setPixel(int x, int y, const RGB& color);
    void setPixel(int x, int y, int r, int g, int b);
    
    // Método para crear copia
    PPMImage* clone() const;
    
private:
    // Métodos auxiliares
    void allocateMemory();
    void deallocateMemory();
    void copyPixels(const PPMImage& other);
};

#endif // PPMIMAGE_H