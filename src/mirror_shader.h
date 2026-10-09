
#ifndef MIRROR_SHADER_H
#define MIRROR_SHADER_H

#include "shader.h"
#include "ray.h"

class Scene;

class MirrorShader : public Shader {
public:
    explicit MirrorShader(
        const vec3& reflectance = vec3(1, 1, 1)
    );

    vec3 rayColor(
        const HitRecord& hit
    ) const override;

    vec3 rayColor(
        const Scene& scene,
        const ray& incoming,
        const HitRecord& hit,
        int remainingDepth
    ) const;

private:
    vec3 reflectance;
};

#endif
