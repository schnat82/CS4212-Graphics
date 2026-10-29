
#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include "ray.h"
#include "aabb.h"

class Shader;

struct HitRecord {
    double t;
    point p;
    vec3 normal;
    vec3 viewDirection;
    std::shared_ptr<Shader> shader;
};

class Shape {
public:
    virtual ~Shape() = default;

    virtual bool intersect(
        const ray& r,
        double tMin,
        double tMax,
        HitRecord& hit
    ) const = 0;

    virtual AABB boundingBox() const = 0;

    void setShader(std::shared_ptr<Shader> shaderIn) {
        shader = shaderIn;
    }

    std::shared_ptr<Shader> getShader() const {
        return shader;
    }

protected:
    std::shared_ptr<Shader> shader;
};

#endif
