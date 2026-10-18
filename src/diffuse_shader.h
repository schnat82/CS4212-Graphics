
#ifndef DIFFUSE_SHADER_H
#define DIFFUSE_SHADER_H

#include "shader.h"
#include "ray.h"

class Scene;

class DiffuseShader : public Shader {
public:
    explicit DiffuseShader(const vec3& albedo)
        : albedo_(albedo) {
    }

    vec3 rayColor(const HitRecord& hit) const override;

    vec3 rayColor(
        const Scene& scene,
        const ray& incoming,
        const HitRecord& hit,
        int remainingDepth
    ) const;

private:
    vec3 albedo_;
};

#endif
