#include "sphere.h"

#include <cmath>

Sphere::Sphere(const point& center, double radius)
    : sphereCenter(center), sphereRadius(radius) {
}

bool Sphere::intersect(const ray& r) const {
    vec3 oc = r.origin() - sphereCenter;

    double a = dot(r.direction(), r.direction());
    double b = 2.0 * dot(oc, r.direction());
    double c = dot(oc, oc) - sphereRadius * sphereRadius;

    double discriminant = b * b - 4.0 * a * c;

    return discriminant >= 0.0;
}

const point& Sphere::center() const {
    return sphereCenter;
}

double Sphere::radius() const {
    return sphereRadius;
}
