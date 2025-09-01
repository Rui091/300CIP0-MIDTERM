#ifndef OMPFILTER_H
#define OMPFILTER_H

#include "Filter.h"
#include <omp.h>

class OMPFilter {
public:
    static Image* applyFilterParallel(const Image* input, Filter* filter);
    
private:
    static void applyKernelParallel(const Image* input, Image* output, 
                                  double kernel[3][3], bool isLaplacian);
};

#endif // OMPFILTER_H
