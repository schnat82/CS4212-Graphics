#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape.h"

class Triangle : public Shape {
public:
    Triangle(
        const point& a,
        const point& b,
        const point& c
    );

    bool intersect(
        const ray& r,
        double tMin,
        double tMax,
        HitRecord& hit
    ) const override;

private:
    point vertexA;
    point vertexB;
    point vertexC;
};

#endif