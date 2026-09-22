#include <algorithm>

#include "png++/png.hpp"
#include "camera.h"
#include "vec3.h"

int main()
{
    const int imageWidth = 200;
    const int imageHeight = 200;

    point cameraPosition(2.0, 1.0, 3.0);
    vec3 viewDirection(-2.0, -0.5, -3.0);

    double imagePlaneWidth = 2.0;
    double imagePlaneHeight = 2.0;
    double focalLength = 1.0;

    PerspectiveCamera camera(
        cameraPosition,
        viewDirection,
        imagePlaneWidth,
        imagePlaneHeight,
        focalLength,
        imageWidth,
        imageHeight
    );

    png::image<png::rgb_pixel> image(imageWidth, imageHeight);

    for (int j = 0; j < imageHeight; ++j)
    {
        for (int i = 0; i < imageWidth; ++i)
        {
            ray r = camera.generateRay(i, j);

            vec3 direction = unit_vector(r.direction());

            // Convert direction components from [-1, 1] to [0, 1].
            double red   = 0.5 * (direction.x() + 1.0);
            double green = 0.5 * (direction.y() + 1.0);
            double blue  = 0.5 * (direction.z() + 1.0);

            png::byte rByte = static_cast<png::byte>(
                std::clamp(red * 255.0, 0.0, 255.0)
            );

            png::byte gByte = static_cast<png::byte>(
                std::clamp(green * 255.0, 0.0, 255.0)
            );

            png::byte bByte = static_cast<png::byte>(
                std::clamp(blue * 255.0, 0.0, 255.0)
            );

            image[j][i] = png::rgb_pixel(rByte, gByte, bByte);
        }
    }

    image.write("ray_direction.png");

    return 0;
}