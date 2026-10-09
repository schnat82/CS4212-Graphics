
#include "scene.h"
#include "shader.h"
#include "mirror_shader.h"

Scene::Scene(
    const vec3& background,
    const point& light
)
    : backgroundColor(background),
      lightPosition(light) {
}

void Scene::add(const std::shared_ptr<Shape>& shape) {
    objects.push_back(shape);
}

bool Scene::occluded(
    const point& position,
    const vec3& normal
) const {
    const vec3 toLight = lightPosition - position;
    const double distance = toLight.length();

    const ray shadowRay(
        position + 0.001 * normal,
        toLight / distance
    );

    for (const auto& object : objects) {
        HitRecord hit;

        if (object->intersect(
            shadowRay,
            0.001,
            distance - 0.001,
            hit
        )) {
            return true;
        }
    }

    return false;
}

vec3 Scene::computeRayColor(
    const ray& r,
    double tMin,
    double tMax,
    int depth
) const {
    // Stop rays from bouncing indefinitely.
    if (depth <= 0) {
        return vec3(0, 0, 0);
    }

    HitRecord closestHit{};
    bool found = false;
    double closestT = tMax;

    // Find the closest shape hit by the ray.
    for (const auto& object : objects) {
        HitRecord candidate{};

        if (object->intersect(
            r,
            tMin,
            closestT,
            candidate
        )) {
            found = true;
            closestT = candidate.t;
            closestHit = candidate;

            // Some existing shapes, such as Triangle,
            // do not populate the shader in intersect().
            closestHit.shader = object->getShader();

            closestHit.viewDirection =
                unit_vector(-r.direction());
        }
    }

    // Nothing was hit, so return the background.
    if (!found) {
        return backgroundColor;
    }

    if (!closestHit.shader) {
        return vec3(1, 0, 1);
    }

    // Mirrors generate another ray into the scene.
    if (const auto* mirror =
            dynamic_cast<const MirrorShader*>(
                closestHit.shader.get()
            )) {
        return mirror->rayColor(
            *this,
            r,
            closestHit,
            depth - 1
        );
    }

    // Existing Lambertian and Blinn-Phong materials
    // retain their direct-lighting behavior.
    if (occluded(
        closestHit.p,
        closestHit.normal
    )) {
        return vec3(0.025, 0.025, 0.025);
    }

    return closestHit.shader->rayColor(closestHit);
}
