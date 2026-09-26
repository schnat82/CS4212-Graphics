#include "sphere.h"

#include <cmath>

Sphere::Sphere(const point& center, double radius)
    : sphereCenter(center), sphereRadius(radius) {
}

bool Sphere::intersect(
    const ray& r,
    double tMin,
    double tMax,
    HitRecord& hit
) const {
    vec3 oc = r.origin() - sphereCenter;

    double a = dot(r.direction(), r.direction());
    double b = 2.0 * dot(oc, r.direction());
    double c = dot(oc, oc) - sphereRadius * sphereRadius;

    double discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) {
        return false;
    }

    double sqrtDiscriminant = std::sqrt(discriminant);

    double t = (-b - sqrtDiscriminant) / (2.0 * a);

    if (t < tMin || t > tMax) {
        t = (-b + sqrtDiscriminant) / (2.0 * a);

        if (t < tMin || t > tMax) {
            return false;
        }
    }

    hit.t = t;
    hit.p = r.at(t);
    hit.normal = unit_vector(hit.p - sphereCenter);

    return true;
}

const point& Sphere::center() const {
    return sphereCenter;
}

double Sphere::radius() const {
    return sphereRadius;
}