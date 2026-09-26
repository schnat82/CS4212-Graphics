#include <algorithm>

#include "camera.h"
#include "framebuffer.h"
#include "png++/png.hpp"
#include "sphere.h"
#include <limits>

int main()
{
    const int width = 400;
    const int height = 240;

    Framebuffer framebuffer(width, height);

    PerspectiveCamera camera(
        point(0, 0, 0),
        vec3(0, 0, -1),
        2.0,
        1.2,
        1.0,
        width,
        height
    );

    Sphere circle(point(0, 0, -3), 0.75);

    const vec3 backgroundColor(1.0, 1.0, 1.0);
    const vec3 circleColor(0.74, 0.0, 0.18);

    for (int y = 0; y < framebuffer.height(); ++y) {
        for (int x = 0; x < framebuffer.width(); ++x) {
            ray r = camera.generateRay(x, y);

            HitRecord hit;

        if (circle.intersect(r, 0.001, std::numeric_limits<double>::infinity(), hit )) {
                framebuffer.setPixel(x, y, circleColor);
            } else {
                framebuffer.setPixel(x, y, backgroundColor);
            }
        }
    }

    png::image<png::rgb_pixel> image(width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
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

    image.write("sphere_flag.png");

    return 0;
}
