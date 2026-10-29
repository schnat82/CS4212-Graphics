
#ifndef SPHERE_H
#define SPHERE_H

#include "shape.h"
#include "vec3.h"
#include <cmath>

class Sphere : public Shape {
public:
    Sphere(const point& center, double radius);

    bool intersect(
        const ray& r,
        double tMin,
        double tMax,
        HitRecord& hit
    ) const override;

    AABB boundingBox() const override {
        const double r = std::abs(sphereRadius);
        const vec3 offset(r, r, r);

        return AABB(
            sphereCenter - offset,
            sphereCenter + offset
        );
    }

    const point& center() const;
    double radius() const;

private:
    point sphereCenter;
    double sphereRadius;
};

#endif
