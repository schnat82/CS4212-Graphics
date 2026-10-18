
#include "lambertian_shader.h"

#include <algorithm>
#include "shape.h"

LambertianShader::LambertianShader(
    const vec3& diffuseColor,
    const point& lightPosition
)
    : kd(diffuseColor),
      lightPosition(lightPosition) {
}

vec3 LambertianShader::rayColor(
    const HitRecord& hit
) const {
    return rayColorAtLight(hit, lightPosition);
}

vec3 LambertianShader::rayColorAtLight(
    const HitRecord& hit,
    const point& light
) const {
    const vec3 lightDir =
        unit_vector(light - hit.p);

    const double nDotL =
        std::max(0.0, dot(hit.normal, lightDir));

    return kd * nDotL;
}
