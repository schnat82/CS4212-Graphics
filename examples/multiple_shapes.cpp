#include <algorithm>
#include <limits>
#include <memory>
#include <vector>

#include "camera.h"
#include "framebuffer.h"
#include "png++/png.hpp"
#include "shape.h"
#include "sphere.h"
#include "triangle.h"

int main()
{
    const int width = 200;
    const int height = 200;

    Framebuffer framebuffer(width, height);

    PerspectiveCamera camera(
        point(0, 0.4, 1.5),
        vec3(0, -0.1, -1),
        2.0,
        2.0,
        1.0,
        width,
        height
    );

    std::vector<std::shared_ptr<Shape>> shapes;

    // Left sphere
    shapes.push_back(
        std::make_shared<Sphere>(
            point(-0.55, 0.0, -3.0),
            0.65
        )
    );

    // Sphere slightly behind the first sphere
    shapes.push_back(
        std::make_shared<Sphere>(
            point(0.35, 0.15, -3.8),
            0.85
        )
    );

    // Triangle crossing in front of part of the spheres
    shapes.push_back(
        std::make_shared<Triangle>(
            point(-0.9, -0.8, -2.5),
            point(0.9, -0.7, -2.8),
            point(0.15, 0.9, -2.7)
        )
    );

    // Smaller triangle farther back
    shapes.push_back(
        std::make_shared<Triangle>(
            point(-0.2, -0.3, -4.5),
            point(1.0, -0.4, -4.4),
            point(0.65, 0.8, -4.6)
        )
    );

    const vec3 backgroundColor(0.85, 0.90, 1.0);

    for (int y = 0; y < framebuffer.height(); ++y) {

        for (int x = 0; x < framebuffer.width(); ++x) {

            ray r = camera.generateRay(x, y);

            double closestT =
                std::numeric_limits<double>::infinity();

            bool hitAnything = false;

            HitRecord closestHit;

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

            if (hitAnything) {

                // Normal visualization:
                // convert [-1,1] normal components into [0,1] RGB.
                vec3 color =
                    0.5 * (
                        closestHit.normal
                        + vec3(1.0, 1.0, 1.0)
                    );

                framebuffer.setPixel(
                    x,
                    y,
                    color
                );

            } else {

                framebuffer.setPixel(
                    x,
                    y,
                    backgroundColor
                );
            }
        }
    }

    png::image<png::rgb_pixel> image(width, height);

    for (int y = 0; y < height; ++y) {

        for (int x = 0; x < width; ++x) {

            vec3 color =
                framebuffer.getPixel(x, y);

            png::byte r =
                static_cast<png::byte>(
                    255.0 *
                    std::clamp(
                        color.x(),
                        0.0,
                        1.0
                    )
                );

            png::byte g =
                static_cast<png::byte>(
                    255.0 *
                    std::clamp(
                        color.y(),
                        0.0,
                        1.0
                    )
                );

            png::byte b =
                static_cast<png::byte>(
                    255.0 *
                    std::clamp(
                        color.z(),
                        0.0,
                        1.0
                    )
                );

            image[y][x] =
                png::rgb_pixel(r, g, b);
        }
    }

    image.write("multiple_shapes.png");

    return 0;
}