
#ifndef NORMAL_SHADER_H
#define NORMAL_SHADER_H

#include "shader.h"
#include "shape.h"

class NormalShader : public Shader {
public:
    vec3 rayColor(const HitRecord& hit) const override {
        const vec3 n = unit_vector(hit.normal);

        return 0.5 * (n + vec3(1, 1, 1));
    }
};

#endif
