#include "ray.h"

ray::ray()
    : orig(point(0, 0, 0)), dir(vec3(0, 0, 0)) {
}

ray::ray(const point& origin, const vec3& direction)
    : orig(origin), dir(direction) {
}

const point& ray::origin() const {
    return orig;
}

const vec3& ray::direction() const {
    return dir;
}

point ray::at(double t) const {
    return orig + t * dir;
}