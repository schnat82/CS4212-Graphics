#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
    ray();

    ray(const point& origin, const vec3& direction);

    const point& origin() const;
    const vec3& direction() const;

    point at(double t) const;

private:
    point orig;
    vec3 dir;
};

#endif