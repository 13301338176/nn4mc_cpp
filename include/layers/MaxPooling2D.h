#ifndef _MAXPOOLING2D_H
#define _MAXPOOLING2D_H

#include "Weights.h"

class MaxPooling2D
{
    private:
        std::vector<int> pool_size;
        std::vector<int> strides;
        std::string padding;
        std::string data_format;
};

#endif