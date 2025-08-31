#ifndef PGMIMAGE_H
#define PGMIMAGE_H

#include "Image.h"

class PGMImage : public Image {
private:
    int** pixels;  // Matriz de píxeles (escala de grises)
    
public:
    // Constructor
    PGMImage();
    PGMImage(int w, int h, int max);
    
    // Destructor
    ~PGMImage();
    
    // Constructor de copia
    PGMImage(const PGMImage& other);
    
    // Operador de asignación
    PGMImage& operator=(const PGMImage& other);
    
    // Implementación de métodos virtuales
    bool readFromFile(const std::string& filename) override;
    bool writeToFile(const std::string& filename) const override;
    
    // Métodos específicos de PGM
    int getPixel(int x, int y) const;
    void setPixel(int x, int y, int value);
    
    // Método para crear copia
    PGMImage* clone() const;
    
private:
    // Métodos auxiliares
    void allocateMemory();
    void deallocateMemory();
    void copyPixels(const PGMImage& other);
};

#endif // PGMIMAGE_H