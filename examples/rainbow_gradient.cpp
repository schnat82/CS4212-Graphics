#include <algorithm>
#include <cmath>

#include "png++/png.hpp"
#include "vec3.h"

// Linear interpolation between two colors.
vec3 lerp(const vec3& start, const vec3& end, double t)
{
    return (1.0 - t) * start + t * end;
}

// Convert a value from 0.0 to 1.0 into a rainbow color.
vec3 rainbow(double t)
{
    const vec3 red(1.0, 0.0, 0.0);
    const vec3 yellow(1.0, 1.0, 0.0);
    const vec3 green(0.0, 1.0, 0.0);
    const vec3 cyan(0.0, 1.0, 1.0);
    const vec3 blue(0.0, 0.0, 1.0);
    const vec3 magenta(1.0, 0.0, 1.0);

    // Six sections of the spectrum.
    double scaled = t * 6.0;

    if (scaled < 1.0)
        return lerp(red, yellow, scaled);

    if (scaled < 2.0)
        return lerp(yellow, green, scaled - 1.0);

    if (scaled < 3.0)
        return lerp(green, cyan, scaled - 2.0);

    if (scaled < 4.0)
        return lerp(cyan, blue, scaled - 3.0);

    if (scaled < 5.0)
        return lerp(blue, magenta, scaled - 4.0);

    return lerp(magenta, red, scaled - 5.0);
}

int main()
{
    const int width = 200;
    const int height = 200;

    png::image<png::rgb_pixel> image(width, height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // Horizontal position from 0 to 1.
            double t = static_cast<double>(x) / (width - 1);

            vec3 color = rainbow(t);

            png::byte r =
                static_cast<png::byte>(255.0 * color.x());
            png::byte g =
                static_cast<png::byte>(255.0 * color.y());
            png::byte b =
                static_cast<png::byte>(255.0 * color.z());

            image[y][x] = png::rgb_pixel(r, g, b);
        }
    }

    image.write("rainbow_gradient.png");

    return 0;
}