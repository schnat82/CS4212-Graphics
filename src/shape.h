#ifndef SHAPE_H
#define SHAPE_H

#include "ray.h"

struct HitRecord {
    double t;
    point p;
    vec3 normal;
};

class Shape {
public:
    virtual ~Shape() = default;

    virtual bool intersect(
        const ray& r,
        double tMin,
        double tMax,
        HitRecord& hit
    ) const = 0;
};

#endif