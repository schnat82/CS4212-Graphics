#include "triangle.h"

#include <cmath>

Triangle::Triangle(
    const point& a,
    const point& b,
    const point& c
)
    : vertexA(a),
      vertexB(b),
      vertexC(c) {
}

bool Triangle::intersect(
    const ray& r,
    double tMin,
    double tMax,
    HitRecord& hit
) const {
    const double epsilon = 1e-8;

    vec3 edge1 = vertexB - vertexA;
    vec3 edge2 = vertexC - vertexA;

    vec3 h = cross(r.direction(), edge2);
    double determinant = dot(edge1, h);

    if (std::abs(determinant) < epsilon) {
        return false;
    }

    double inverseDeterminant = 1.0 / determinant;

    vec3 s = r.origin() - vertexA;

    double beta = inverseDeterminant * dot(s, h);

    if (beta < 0.0 || beta > 1.0) {
        return false;
    }

    vec3 q = cross(s, edge1);

    double gamma =
        inverseDeterminant * dot(r.direction(), q);

    if (gamma < 0.0 || beta + gamma > 1.0) {
        return false;
    }

    double t =
        inverseDeterminant * dot(edge2, q);

    if (t < tMin || t > tMax) {
        return false;
    }

    hit.t = t;
    hit.p = r.at(t);

    hit.normal =
        unit_vector(cross(edge1, edge2));

    return true;
}