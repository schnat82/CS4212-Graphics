#include <algorithm>
#include <cmath>

#include "framebuffer.h"
#include "png++/png.hpp"
#include "vec3.h"

double distanceToSegment(
    double px, double py,
    double ax, double ay,
    double bx, double by
) {
    double abx = bx - ax;
    double aby = by - ay;

    double apx = px - ax;
    double apy = py - ay;

    double lengthSquared = abx * abx + aby * aby;

    double t = (apx * abx + apy * aby) / lengthSquared;
    t = std::clamp(t, 0.0, 1.0);

    double closestX = ax + t * abx;
    double closestY = ay + t * aby;

    double dx = px - closestX;
    double dy = py - closestY;

    return std::sqrt(dx * dx + dy * dy);
}

bool nearLine(
    double px, double py,
    double ax, double ay,
    double bx, double by,
    double thickness
) {
    return distanceToSegment(
        px, py,
        ax, ay,
        bx, by
    ) < thickness;
}

int main()
{
    const int width = 800;
    const int height = 500;

    Framebuffer framebuffer(width, height);

    const vec3 black(0.0, 0.0, 0.0);
    const vec3 white(1.0, 1.0, 1.0);

    // Original prism position.
    const double topX = 400;
    const double topY = 110;

    const double leftX = 300;
    const double leftY = 350;

    const double rightX = 500;
    const double rightY = 350;

    // Rainbow colors.
    const vec3 rainbow[] = {
        vec3(1.0, 0.1, 0.1),
        vec3(1.0, 0.5, 0.0),
        vec3(1.0, 1.0, 0.0),
        vec3(0.1, 1.0, 0.2),
        vec3(0.1, 0.6, 1.0),
        vec3(0.3, 0.2, 1.0),
        vec3(0.7, 0.1, 1.0)
    };

    // Start with black background.
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            framebuffer.setPixel(x, y, black);
        }
    }

    // Draw everything using pixel tests.
    for (int y = 0; y < height; ++y) {

        for (int x = 0; x < width; ++x) {

            double px = static_cast<double>(x);
            double py = static_cast<double>(y);

            vec3 color = framebuffer.getPixel(x, y);

            // Incoming white beam.
            if (nearLine(
                    px, py,
                    40, 220,
                    335, 250,
                    3.0
                )) {
                color = white;
            }

            // Prism edges.
            if (
                nearLine(
                    px, py,
                    topX, topY,
                    leftX, leftY,
                    2.0
                ) ||
                nearLine(
                    px, py,
                    leftX, leftY,
                    rightX, rightY,
                    2.0
                ) ||
                nearLine(
                    px, py,
                    rightX, rightY,
                    topX, topY,
                    2.0
                )
            ) {
                color = white;
            }

            // Rainbow leaving prism.
            for (int i = 0; i < 7; ++i) {

                double startX = 465;
                double startY = 260;

                double endX = 770;
                double endY = 180 + i * 35;

                if (nearLine(
                        px, py,
                        startX, startY,
                        endX, endY,
                        4.0
                    )) {
                    color = rainbow[i];
                }
            }

            framebuffer.setPixel(x, y, color);
        }
    }

    // Convert framebuffer into PNG.
    png::image<png::rgb_pixel> image(width, height);

    for (int y = 0; y < height; ++y) {

        for (int x = 0; x < width; ++x) {

            vec3 color = framebuffer.getPixel(x, y);

            png::byte r =
                static_cast<png::byte>(
                    255.0 *
                    std::clamp(color.x(), 0.0, 1.0)
                );

            png::byte g =
                static_cast<png::byte>(
                    255.0 *
                    std::clamp(color.y(), 0.0, 1.0)
                );

            png::byte b =
                static_cast<png::byte>(
                    255.0 *
                    std::clamp(color.z(), 0.0, 1.0)
                );

            image[y][x] =
                png::rgb_pixel(r, g, b);
        }
    }

    image.write("prism_rainbow.png");

    return 0;
}