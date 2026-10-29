
#ifndef CS4212_AABB_H
#define CS4212_AABB_H

#include <algorithm>
#include <cmath>
#include "ray.h"

struct AABB {
    point minimum;
    point maximum;

    AABB() = default;

    AABB(const point& a, const point& b)
        : minimum(a), maximum(b) {}

    bool hit(const ray& r, double tMin, double tMax) const {
        for (int axis = 0; axis < 3; ++axis) {
            const double direction = r.direction()[axis];

            if (std::abs(direction) < 1e-15) {
                if (r.origin()[axis] < minimum[axis] ||
                    r.origin()[axis] > maximum[axis]) {
                    return false;
                }
                continue;
            }

            double t0 =
                (minimum[axis] - r.origin()[axis]) / direction;

            double t1 =
                (maximum[axis] - r.origin()[axis]) / direction;

            if (t0 > t1) {
                std::swap(t0, t1);
            }

            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);

            if (tMax < tMin) {
                return false;
            }
        }

        return true;
    }
};

inline AABB surroundingBox(const AABB& a, const AABB& b) {
    point minimum;
    point maximum;

    for (int axis = 0; axis < 3; ++axis) {
        minimum[axis] = std::min(
            a.minimum[axis], b.minimum[axis]
        );

        maximum[axis] = std::max(
            a.maximum[axis], b.maximum[axis]
        );
    }

    return AABB(minimum, maximum);
}

#endif
