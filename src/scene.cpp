
#include "scene.h"
#include "bvh.h"
#include "shader.h"
#include "mirror_shader.h"
#include "lambertian_shader.h"
#include "blinn_phong_shader.h"
#include "normal_shader.h"
#include "diffuse_shader.h"

Scene::Scene(
    const vec3& background,
    const point& light
) : backgroundColor(background) {
    addLight(light, vec3(1, 1, 1));
}

void Scene::add(const std::shared_ptr<Shape>& shape) {
    objects.push_back(shape);
    bvhRoot.reset();
}

void Scene::addLight(
    const point& position,
    const vec3& intensity
) {
    lights.push_back({position, intensity});
}

void Scene::clearLights() {
    lights.clear();
}

void Scene::setUseBVH(bool enabled) {
    useBVH = enabled;
}

void Scene::buildBVH() {
    bvhRoot.reset();

    if (!objects.empty()) {
        bvhRoot = std::make_shared<BVHNode>(objects);
    }
}

bool Scene::closestIntersection(
    const ray& r,
    double tMin,
    double tMax,
    HitRecord& hit
) const {
    if (useBVH && bvhRoot) {
        return bvhRoot->intersect(r, tMin, tMax, hit);
    }

    bool found = false;
    double closestT = tMax;

    for (const auto& object : objects) {
        HitRecord candidate{};

        if (object->intersect(
            r, tMin, closestT, candidate
        )) {
            found = true;
            closestT = candidate.t;
            candidate.shader = object->getShader();
            hit = candidate;
        }
    }

    return found;
}

bool Scene::occludedByLight(
    const point& position,
    const vec3& normal,
    const point& light
) const {
    const vec3 toLight = light - position;
    const double distance = toLight.length();

    if (distance <= 0.001) {
        return false;
    }

    const ray shadowRay(
        position + 0.001 * normal,
        toLight / distance
    );

    HitRecord hit{};

    return closestIntersection(
        shadowRay,
        0.001,
        distance - 0.001,
        hit
    );
}

bool Scene::occluded(
    const point& position,
    const vec3& normal
) const {
    return !lights.empty() &&
        occludedByLight(
            position,
            normal,
            lights.front().position
        );
}

vec3 Scene::computeRayColor(
    const ray& r,
    double tMin,
    double tMax,
    int depth
) const {
    if (depth <= 0) {
        return vec3(0, 0, 0);
    }

    HitRecord closestHit{};

    if (!closestIntersection(
        r, tMin, tMax, closestHit
    )) {
        return backgroundColor;
    }

    closestHit.viewDirection =
        unit_vector(-r.direction());

    if (!closestHit.shader) {
        return vec3(1, 0, 1);
    }

    // Mirror reflections
    if (const auto* mirror =
        dynamic_cast<const MirrorShader*>(
            closestHit.shader.get()
        )) {
        return mirror->rayColor(
            *this, r, closestHit, depth - 1
        );
    }

    // Normal visualization
    if (dynamic_cast<const NormalShader*>(
        closestHit.shader.get()
    )) {
        return closestHit.shader->rayColor(closestHit);
    }

    // Diffuse scattering
    if (const auto* diffuse =
        dynamic_cast<const DiffuseShader*>(
            closestHit.shader.get()
        )) {
        return diffuse->rayColor(
            *this, r, closestHit, depth - 1
        );
    }

    // Direct lighting
    vec3 color(0, 0, 0);

    for (const auto& light : lights) {
        if (occludedByLight(
            closestHit.p,
            closestHit.normal,
            light.position
        )) {
            continue;
        }

        vec3 contribution(0, 0, 0);

        if (const auto* lambert =
            dynamic_cast<const LambertianShader*>(
                closestHit.shader.get()
            )) {
            contribution = lambert->rayColorAtLight(
                closestHit, light.position
            );
        } else if (const auto* blinn =
            dynamic_cast<const BlinnPhongShader*>(
                closestHit.shader.get()
            )) {
            contribution = blinn->rayColorAtLight(
                closestHit, light.position
            );
        } else {
            contribution =
                closestHit.shader->rayColor(closestHit);
        }

        color += contribution * light.intensity;
    }

    return color;
}
