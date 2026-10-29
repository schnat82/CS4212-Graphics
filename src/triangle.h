
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape.h"
#include <algorithm>

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

    AABB boundingBox() const override {
        point low;
        point high;

        for (int axis = 0; axis < 3; ++axis) {
            low[axis] = std::min({
                vertexA[axis],
                vertexB[axis],
                vertexC[axis]
            }) - 1e-5;

            high[axis] = std::max({
                vertexA[axis],
                vertexB[axis],
                vertexC[axis]
            }) + 1e-5;
        }

        return AABB(low, high);
    }

private:
    point vertexA;
    point vertexB;
    point vertexC;
};

#endif
