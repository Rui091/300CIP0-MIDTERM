#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Image {
protected:
    int width;
    int height;
    int maxVal;
    std::string magicNumber;
    
public:
    // Constructor
    Image();
    Image(int w, int h, int max);
    
    // Destructor virtual para polimorfismo
    virtual ~Image() = default;
    
    // Métodos virtuales puros
    virtual bool readFromFile(const std::string& filename) = 0;
    virtual bool writeToFile(const std::string& filename) const = 0;
    virtual void displayInfo() const;
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getMaxVal() const { return maxVal; }
    std::string getMagicNumber() const { return magicNumber; }
    
    // Setters
    void setWidth(int w) { width = w; }
    void setHeight(int h) { height = h; }
    void setMaxVal(int max) { maxVal = max; }
    void setMagicNumber(const std::string& magic) { magicNumber = magic; }
    
protected:
    // Método auxiliar para leer comentarios
    void skipComments(std::ifstream& file) const;
};

#endif // IMAGE_H