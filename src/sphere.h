#ifndef SPHERE_H
#define SPHERE_H

#include "shape.h"
#include "vec3.h"

class Sphere : public Shape {
public:
    Sphere(const point& center, double radius);

    bool intersect(
        const ray& r,
        double tMin,
        double tMax,
        HitRecord& hit
    ) const override;

    const point& center() const;
    double radius() const;

private:
    point sphereCenter;
    double sphereRadius;
};

#endif