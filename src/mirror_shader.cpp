
#include "mirror_shader.h"
#include "scene.h"
#include "shape.h"

#include <limits>

MirrorShader::MirrorShader(
    const vec3& color
)
    : reflectance(color) {
}

// Required by the original Shader interface.
// The Scene calls the recursive overload below.
vec3 MirrorShader::rayColor(
    const HitRecord&
) const {
    return vec3(0, 0, 0);
}

vec3 MirrorShader::rayColor(
    const Scene& scene,
    const ray& incoming,
    const HitRecord& hit,
    int remainingDepth
) const {
    // Stop recursive reflection when depth runs out.
    if (remainingDepth <= 0) {
        return vec3(0, 0, 0);
    }

    // Incoming direction and surface normal.
    const vec3 d = unit_vector(incoming.direction());
    const vec3 n = unit_vector(hit.normal);

    // Reflection formula:
    // r = d - 2 * dot(d, n) * n
    const vec3 reflected =
        d - 2.0 * dot(d, n) * n;

    // Offset the new ray away from the surface.
    const vec3 orientedNormal =
        dot(reflected, n) >= 0.0 ? n : -n;

    const ray reflectionRay(
        hit.p + 0.001 * orientedNormal,
        reflected
    );

    // Recursively trace the reflection ray.
    return reflectance * scene.computeRayColor(
        reflectionRay,
        0.001,
        std::numeric_limits<double>::infinity(),
        remainingDepth
    );
}
