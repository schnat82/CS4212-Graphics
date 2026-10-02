#include <algorithm>
#include <limits>
#include <memory>

#include "blinn_phong_shader.h"
#include "camera.h"
#include "framebuffer.h"
#include "lambertian_shader.h"
#include "png++/png.hpp"
#include "sphere.h"

void writeFramebuffer(
    const Framebuffer& framebuffer,
    const char* filename
) {
    png::image<png::rgb_pixel> image(
        framebuffer.width(),
        framebuffer.height()
    );

    for (int y = 0; y < framebuffer.height(); ++y) {
        for (int x = 0; x < framebuffer.width(); ++x) {
            vec3 color = framebuffer.getPixel(x, y);

            png::byte r = static_cast<png::byte>(
                255.0 * std::clamp(color.x(), 0.0, 1.0)
            );
            png::byte g = static_cast<png::byte>(
                255.0 * std::clamp(color.y(), 0.0, 1.0)
            );
            png::byte b = static_cast<png::byte>(
                255.0 * std::clamp(color.z(), 0.0, 1.0)
            );

            image[y][x] = png::rgb_pixel(r, g, b);
        }
    }

    image.write(filename);
}

void renderSphere(
    Framebuffer& framebuffer,
    PerspectiveCamera& camera,
    Sphere& sphere,
    const vec3& background
) {
    for (int y = 0; y < framebuffer.height(); ++y) {
        for (int x = 0; x < framebuffer.width(); ++x) {
            ray r = camera.generateRay(x, y);
            HitRecord hit;

            if (sphere.intersect(
                    r,
                    0.001,
                    std::numeric_limits<double>::infinity(),
                    hit
                )) {
                framebuffer.setPixel(
                    x,
                    y,
                    hit.shader->rayColor(hit)
                );
            } else {
                framebuffer.setPixel(x, y, background);
            }
        }
    }
}

int main()
{
    const int width = 400;
    const int height = 400;

    PerspectiveCamera camera(
        point(0, 0, 0),
        vec3(0, 0, -1),
        2.0,
        2.0,
        1.0,
        width,
        height
    );

    const vec3 background(0.05, 0.05, 0.08);
    const point lightPosition(0, 10, 5);

    {
        Framebuffer framebuffer(width, height);

        auto shader = std::make_shared<LambertianShader>(
            vec3(0.15, 0.55, 0.95),
            lightPosition
        );

        Sphere sphere(point(0, 0, -3), 0.9);
        sphere.setShader(shader);

        renderSphere(framebuffer, camera, sphere, background);
        writeFramebuffer(framebuffer, "lambertian_sphere.png");
    }

    {
        Framebuffer framebuffer(width, height);

        auto shader = std::make_shared<BlinnPhongShader>(
            vec3(0.15, 0.55, 0.95),
            vec3(1.0, 1.0, 1.0),
            64.0,
            lightPosition
        );

        Sphere sphere(point(0, 0, -3), 0.9);
        sphere.setShader(shader);

        renderSphere(framebuffer, camera, sphere, background);
        writeFramebuffer(framebuffer, "blinn_phong_sphere.png");
    }

    return 0;
}
