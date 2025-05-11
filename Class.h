#ifndef CLASS_H
#define CLASS_H

#include <vector>

struct Net {
    unsigned int x1;
    unsigned int y1;
    unsigned int x2;
    unsigned int y2;
};

struct Circuit {
    unsigned int girdX;
    unsigned int girdY;
    unsigned int propagationLoss;
    unsigned int crossingLoss;
    unsigned int bendingLoss;
    std::vector<Net> nets;
};

#endif