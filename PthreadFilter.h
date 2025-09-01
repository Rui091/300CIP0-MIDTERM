#ifndef PTHREADFILTER_H
#define PTHREADFILTER_H

#include "Filter.h"
#include <pthread.h>

// Estructura para pasar datos a los hilos
struct ThreadData {
    const Image* inputImage;
    Image* outputImage;
    Filter* filter;
    int startX, endX;  // Región horizontal
    int startY, endY;  // Región vertical
    int threadId;
};

class PthreadFilter {
public:
    static Image* applyFilterParallel(const Image* input, Filter* filter);
    
private:
    static void* threadFunction(void* arg);
    static void applyFilterToRegion(const Image* input, Image* output, Filter* filter,
                                  int startX, int endX, int startY, int endY);
};

#endif // PTHREADFILTER_H
