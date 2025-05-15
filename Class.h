#ifndef CLASS_H
#define CLASS_H

#include <vector>
#include <deque>

struct Point {
    unsigned int x;
    unsigned int y;
};

struct Net {
    unsigned int id;
    std::deque<Point> points;
};


struct Circuit {
    struct Net {
        unsigned int id;
        unsigned int x1;
        unsigned int y1;
        unsigned int x2;
        unsigned int y2;
    };

    unsigned int gridX;
    unsigned int gridY;
    unsigned int propagationLoss;
    unsigned int crossingLoss;
    unsigned int bendingLoss;
    std::vector<Net> nets;
};

#endif