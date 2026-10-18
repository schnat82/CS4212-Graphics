
#include "diffuse_shader.h"

#include "scene.h"
#include "shape.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

namespace {

vec3 randomUnitVector() {
    thread_local std::mt19937 generator(
        std::random_device{}()
    );

    std::uniform_real_distribution<double> uniform(0.0, 1.0);

    const double z = 2.0 * uniform(generator) - 1.0;
    const double angle =
        6.283185307179586 * uniform(generator);

    const double radial =
        std::sqrt(std::max(0.0, 1.0 - z * z));

    return vec3(
        radial * std::cos(angle),
        radial * std::sin(angle),
        z
    );
}

}

vec3 DiffuseShader::rayColor(
    const HitRecord&
) const {
    return albedo_;
}

vec3 DiffuseShader::rayColor(
    const Scene& scene,
    const ray&,
    const HitRecord& hit,
    int remainingDepth
) const {
    if (remainingDepth <= 0) {
        return vec3(0, 0, 0);
    }

    const vec3 n = unit_vector(hit.normal);
    vec3 direction = n + randomUnitVector();

    if (direction.length_squared() < 1e-12) {
        direction = n;
    }

    const ray scattered(
        hit.p + 0.001 * n,
        unit_vector(direction)
    );

    return albedo_ * scene.computeRayColor(
        scattered,
        0.001,
        std::numeric_limits<double>::infinity(),
        remainingDepth
    );
}
