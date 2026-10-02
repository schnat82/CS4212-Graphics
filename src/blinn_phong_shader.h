#ifndef BLINN_PHONG_SHADER_H
#define BLINN_PHONG_SHADER_H

#include "shader.h"

class BlinnPhongShader : public Shader {
public:
    BlinnPhongShader(
        const vec3& diffuseColor,
        const vec3& specularColor,
        double phongExponent,
        const point& lightPosition = point(0, 10, 5)
    );

    vec3 rayColor(const HitRecord& hit) const override;

private:
    vec3 kd;
    vec3 ks;
    double exponent;
    point lightPosition;
};

#endif
