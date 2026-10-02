#ifndef LAMBERTIAN_SHADER_H
#define LAMBERTIAN_SHADER_H

#include "shader.h"

class LambertianShader : public Shader {
public:
    LambertianShader(
        const vec3& diffuseColor,
        const point& lightPosition = point(0, 10, 5)
    );

    vec3 rayColor(const HitRecord& hit) const override;

private:
    vec3 kd;
    point lightPosition;
};

#endif
