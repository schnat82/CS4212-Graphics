
#include "blinn_phong_shader.h"

#include <algorithm>
#include <cmath>
#include "shape.h"

BlinnPhongShader::BlinnPhongShader(
    const vec3& diffuseColor,
    const vec3& specularColor,
    double phongExponent,
    const point& lightPosition
)
    : kd(diffuseColor),
      ks(specularColor),
      exponent(phongExponent),
      lightPosition(lightPosition) {
}

vec3 BlinnPhongShader::rayColor(
    const HitRecord& hit
) const {
    return rayColorAtLight(hit, lightPosition);
}

vec3 BlinnPhongShader::rayColorAtLight(
    const HitRecord& hit,
    const point& light
) const {
    const vec3 lightDir =
        unit_vector(light - hit.p);

    const double nDotL =
        std::max(0.0, dot(hit.normal, lightDir));

    if (nDotL <= 0.0) {
        return vec3(0, 0, 0);
    }

    const vec3 lambertian = kd * nDotL;

    const vec3 halfVector =
        unit_vector(lightDir + hit.viewDirection);

    const double nDotH =
        std::max(0.0, dot(hit.normal, halfVector));

    const vec3 specular =
        ks * std::pow(nDotH, exponent);

    return lambertian + specular;
}
