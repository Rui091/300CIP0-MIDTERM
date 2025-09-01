#ifndef MPIFILTER_H
#define MPIFILTER_H

#include "Filter.h"
#include <mpi.h>

class MPIFilter {
public:
    static Image* applyFilterDistributed(const Image* input, Filter* filter, 
                                       int rank, int size);
    
    static void distributeImage(const Image* image, int rank, int size);
    static Image* gatherImage(const Image* localResult, int width, int height, int maxVal, 
                             const std::string& magicNumber, int rank, int size);
    
private:
    static void processRegion(const Image* input, Image* output, Filter* filter,
                            int startY, int endY);
    static void copyRegion(const Image* source, Image* dest, int srcStartY, int destStartY, 
                          int width, int height);
    static void sendRegion(const Image* image, int startY, int endY, int width, int dest);
    static void receiveRegion(Image* image, int startY, int endY, int width, int source);
};

#endif // MPIFILTER_H
