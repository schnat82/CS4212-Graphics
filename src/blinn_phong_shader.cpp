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

vec3 BlinnPhongShader::rayColor(const HitRecord& hit) const {
    vec3 lightDir = unit_vector(lightPosition - hit.p);
    double nDotL = std::max(0.0, dot(hit.normal, lightDir));

    vec3 lambertian = kd * nDotL;

    vec3 halfVector = unit_vector(lightDir + hit.viewDirection);
    double nDotH = std::max(0.0, dot(hit.normal, halfVector));
    double specularStrength = std::pow(nDotH, exponent);

    vec3 specular = ks * specularStrength;

    return lambertian + specular;
}
