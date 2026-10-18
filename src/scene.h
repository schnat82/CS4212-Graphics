
#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <vector>
#include <limits>

#include "shape.h"

struct PointLight {
    point position;
    vec3 intensity;
};

class Scene {
public:
    explicit Scene(
        const vec3& background = vec3(0.05, 0.07, 0.12),
        const point& light = point(-3, 7, 2)
    );

    void add(const std::shared_ptr<Shape>& shape);

    void addLight(
        const point& position,
        const vec3& intensity
    );

    void clearLights();

    vec3 computeRayColor(
        const ray& r,
        double tMin = 0.001,
        double tMax = std::numeric_limits<double>::infinity(),
        int depth = 4
    ) const;

    bool occluded(
        const point& position,
        const vec3& normal
    ) const;

private:
    bool occludedByLight(
        const point& position,
        const vec3& normal,
        const point& light
    ) const;

    std::vector<std::shared_ptr<Shape>> objects;
    std::vector<PointLight> lights;
    vec3 backgroundColor;
};

#endif
