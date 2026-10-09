#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include "blinn_phong_shader.h"
#include "camera.h"
#include "framebuffer.h"
#include "lambertian_shader.h"
#include "png++/png.hpp"
#include "shape.h"
#include "sphere.h"

int main()
{
    const int width = 600;
    const int height = 400;

    Framebuffer framebuffer(width, height);

    PerspectiveCamera camera(
        point(0, 0.5, 1.5),
        vec3(0, -0.15, -1),
        3.0,
        2.0,
        1.0,
        width,
        height
    );

    // Light above and to the left of the scene.
    const point lightPosition(-3.0, 5.0, 1.0);

    // -------------------------
    // Create shaders
    // -------------------------

    // Matte red Lambertian
    auto redLambertian =
        std::make_shared<LambertianShader>(
            vec3(0.85, 0.15, 0.15),
            lightPosition
        );

    // Matte green Lambertian
    auto greenLambertian =
        std::make_shared<LambertianShader>(
            vec3(0.15, 0.75, 0.25),
            lightPosition
        );

    // Shiny blue Blinn-Phong
    auto blueBlinnPhong =
        std::make_shared<BlinnPhongShader>(
            vec3(0.10, 0.30, 0.85),
            vec3(1.0, 1.0, 1.0),
            64.0,
            lightPosition
        );

    // Ground shader
    auto groundShader =
        std::make_shared<LambertianShader>(
            vec3(0.65, 0.65, 0.65),
            lightPosition
        );

    // -------------------------
    // Create scene
    // -------------------------

    std::vector<std::shared_ptr<Shape>> shapes;

    // Left red Lambertian sphere
    auto leftSphere =
        std::make_shared<Sphere>(
            point(-0.9, 0.0, -3.5),
            0.65
        );

    leftSphere->setShader(redLambertian);
    shapes.push_back(leftSphere);

    // Center shiny Blinn-Phong sphere
    auto centerSphere =
        std::make_shared<Sphere>(
            point(0.15, 0.15, -3.0),
            0.75
        );

    centerSphere->setShader(blueBlinnPhong);
    shapes.push_back(centerSphere);

    // Right green Lambertian sphere
    auto rightSphere =
        std::make_shared<Sphere>(
            point(1.15, -0.1, -4.0),
            0.60
        );

    rightSphere->setShader(greenLambertian);
    shapes.push_back(rightSphere);

    // Huge sphere acts like a floor.
    auto ground =
        std::make_shared<Sphere>(
            point(0, -100.8, -3.5),
            100.0
        );

    ground->setShader(groundShader);
    shapes.push_back(ground);

    const vec3 background(0.05, 0.07, 0.12);

    // -------------------------
    // Render
    // -------------------------

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            ray r = camera.generateRay(x, y);

            double closestT =
                std::numeric_limits<double>::infinity();

            bool hitAnything = false;
            HitRecord closestHit;

            // Find the closest object hit by the camera ray.
            for (const auto& shape : shapes) {

                HitRecord hit;

                if (shape->intersect(
                        r,
                        0.001,
                        closestT,
                        hit
                    )) {

                    hitAnything = true;
                    closestT = hit.t;
                    closestHit = hit;
                }
            }

            if (!hitAnything) {
                framebuffer.setPixel(
                    x,
                    y,
                    background
                );

                continue;
            }

            // -------------------------
            // Shadow ray
            // -------------------------

            vec3 toLight =
                lightPosition - closestHit.p;

            double lightDistance =
                toLight.length();

            vec3 lightDirection =
                unit_vector(toLight);

            // Move slightly away from the surface to
            // prevent the shadow ray from hitting the
            // same object because of floating-point error.
            point shadowOrigin =
                closestHit.p +
                0.001 * closestHit.normal;

            ray shadowRay(
                shadowOrigin,
                lightDirection
            );

            bool inShadow = false;

            for (const auto& shape : shapes) {

                HitRecord shadowHit;

                if (shape->intersect(
                        shadowRay,
                        0.001,
                        lightDistance,
                        shadowHit
                    )) {

                    inShadow = true;
                    break;
                }
            }

            vec3 color;

            if (inShadow) {

                // Small ambient term so shadows are
                // dark without becoming pure black.
                color = vec3(0.025, 0.025, 0.025);

            } else if (closestHit.shader) {

                // Use whichever shader belongs to the
                // object that was hit.
                color =
                    closestHit.shader->rayColor(
                        closestHit
                    );

            } else {

                color = vec3(1.0, 0.0, 1.0);
            }

            framebuffer.setPixel(
                x,
                y,
                color
            );
        }
    }

    // -------------------------
    // Write PNG
    // -------------------------

    png::image<png::rgb_pixel> image(
        width,
        height
    );

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            vec3 color =
                framebuffer.getPixel(x, y);

            png::byte red =
                static_cast<png::byte>(
                    255.0 *
                    std::clamp(
                        color.x(),
                        0.0,
                        1.0
                    )
                );

            png::byte green =
                static_cast<png::byte>(
                    255.0 *
                    std::clamp(
                        color.y(),
                        0.0,
                        1.0
                    )
                );

            png::byte blue =
                static_cast<png::byte>(
                    255.0 *
                    std::clamp(
                        color.z(),
                        0.0,
                        1.0
                    )
                );

            image[y][x] =
                png::rgb_pixel(
                    red,
                    green,
                    blue
                );
        }
    }

    image.write("shadow_scene.png");

    return 0;
}