
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>

#include "png++/png.hpp"

#include "camera.h"
#include "framebuffer.h"
#include "scene.h"
#include "sphere.h"
#include "triangle.h"

#include "lambertian_shader.h"
#include "blinn_phong_shader.h"
#include "mirror_shader.h"

#include "handleGraphicsArgs.h"

#ifdef _OPENMP
#include <omp.h>
#endif

int main(int argc, char** argv) {
    try {
        // -----------------------------------------
        // Command-line arguments
        // -----------------------------------------

        sivelab::GraphicsArgs args;
        args.process(argc, argv);

        if (args.width <= 0 ||
            args.height <= 0 ||
            args.rpp <= 0 ||
            args.recursionDepth <= 0 ||
            args.numCpus <= 0) {

            throw std::invalid_argument(
                "width, height, rpp, recursionDepth "
                "and numcpus must be positive"
            );
        }

        // These options are registered for compatibility
        // with the slides but not implemented here.
       // Reject only unsupported features that were
// explicitly enabled by the user.
if (args.useDepthOfField) {
    throw std::invalid_argument(
        "Depth of field is not implemented"
    );
}

if (args.withPreview) {
    throw std::invalid_argument(
        "OpenGL preview is not implemented"
    );
}

if (args.withGridDim) {
    throw std::invalid_argument(
        "Grid dimension mode is not implemented"
    );
}

if (args.withIntersectTest) {
    throw std::invalid_argument(
        "Intersection-test mode is not implemented"
    );
}

if (!args.inputFileName.empty()) {
    throw std::invalid_argument(
        "Input scene files are not implemented"
    );
}

if (args.splitMethod != "objectMedian") {
    throw std::invalid_argument(
        "Alternative BVH split methods are not implemented"
    );
}

        const int width = args.width;
        const int height = args.height;

        Framebuffer framebuffer(width, height);

        // -----------------------------------------
        // Camera
        // -----------------------------------------

        const double aspect = args.aspectRatio;

        PerspectiveCamera camera(
            point(0, 3.0, 4.0),
            vec3(0, -1.5, -3.0),
            0.5,
            0.5 / aspect,
            0.4,
            width,
            height
        );

        // -----------------------------------------
        // Scene and light
        // -----------------------------------------

        const point light(-3.0, 8.0, 1.0);

        Scene scene(
            vec3(0.07, 0.10, 0.16),
            light
        );

        // -----------------------------------------
        // Shaders
        // -----------------------------------------

        auto groundMat =
            std::make_shared<LambertianShader>(
                vec3(0.65, 0.65, 0.65),
                light
            );

        auto redMat =
            std::make_shared<BlinnPhongShader>(
                vec3(0.85, 0.10, 0.10),
                vec3(1.0, 1.0, 1.0),
                64.0,
                light
            );

        auto blueMat =
            std::make_shared<LambertianShader>(
                vec3(0.12, 0.45, 0.85),
                light
            );

        auto mirrorMat =
            std::make_shared<MirrorShader>();

        // -----------------------------------------
        // Ground plane
        // -----------------------------------------

        // Two large triangles create a floor at y=0.

        auto t1 = std::make_shared<Triangle>(
            point(-100, 0, -100),
            point(-100, 0, 100),
            point(100, 0, -100)
        );

        auto t2 = std::make_shared<Triangle>(
            point(100, 0, -100),
            point(-100, 0, 100),
            point(100, 0, 100)
        );

        t1->setShader(groundMat);
        t2->setShader(groundMat);

        scene.add(t1);
        scene.add(t2);

        // -----------------------------------------
        // Red Blinn-Phong sphere
        // -----------------------------------------

        auto red = std::make_shared<Sphere>(
            point(-1.2, 1.0, -3.0),
            1.0
        );

        red->setShader(redMat);
        scene.add(red);

        // -----------------------------------------
        // Mirror sphere
        // -----------------------------------------

        auto mirror = std::make_shared<Sphere>(
            point(1.2, 1.10, -4.0),
            1.10
        );

        mirror->setShader(mirrorMat);
        scene.add(mirror);

        // -----------------------------------------
        // Additional Lambertian sphere
        // -----------------------------------------

        auto blue = std::make_shared<Sphere>(
            point(0.1, 0.55, -6.5),
            0.55
        );

        blue->setShader(blueMat);
        scene.add(blue);

        // -----------------------------------------
        // Anti-aliasing setup
        // -----------------------------------------

        // --rpp means total rays per pixel.
        // Perfect squares use N x N stratification.

        const int n = static_cast<int>(
            std::sqrt(
                static_cast<double>(args.rpp)
            )
        );

        const bool stratified =
            n * n == args.rpp;

        // -----------------------------------------
        // OpenMP parallel rendering
        // -----------------------------------------

#ifdef _OPENMP
        omp_set_dynamic(0);
        omp_set_num_threads(args.numCpus);

#pragma omp parallel for schedule(dynamic, 1)
#endif
        for (int y = 0; y < height; ++y) {

            // Each row gets its own random generator.
            // This avoids shared RNG state between threads.

            std::seed_seq seed{
                12345u,
                static_cast<unsigned>(y),
                static_cast<unsigned>(width),
                static_cast<unsigned>(height)
            };

            std::mt19937 generator(seed);

            std::uniform_real_distribution<double>
                randomOffset(0.0, 1.0);

            for (int x = 0; x < width; ++x) {

                vec3 accumulated(0, 0, 0);

                // ---------------------------------
                // Multiple rays per pixel
                // ---------------------------------

                for (int sample = 0;
                     sample < args.rpp;
                     ++sample) {

                    const double dx = stratified
                        ? (
                            (sample % n) +
                            randomOffset(generator)
                          ) / n
                        : randomOffset(generator);

                    const double dy = stratified
                        ? (
                            (sample / n) +
                            randomOffset(generator)
                          ) / n
                        : randomOffset(generator);

                    // Generate a ray at a subpixel position.
                    const ray r = camera.generateRay(
                        x + dx,
                        y + dy
                    );

                    // Trace the ray through the scene.
                    accumulated += scene.computeRayColor(
                        r,
                        0.001,
                        std::numeric_limits<double>::infinity(),
                        args.recursionDepth
                    );
                }

                // Average all samples for this pixel.
                framebuffer.setPixel(
                    x,
                    y,
                    accumulated / args.rpp
                );
            }
        }

        // -----------------------------------------
        // Write image to PNG
        // -----------------------------------------

        png::image<png::rgb_pixel> image(
            width,
            height
        );

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {

                const vec3 c =
                    framebuffer.getPixel(x, y);

                const auto channel =
                    [](double v) -> png::byte {

                    return static_cast<png::byte>(
                        255.0 * std::clamp(
                            v,
                            0.0,
                            1.0
                        )
                    );
                };

                image[y][x] = png::rgb_pixel(
                    channel(c.x()),
                    channel(c.y()),
                    channel(c.z())
                );
            }
        }

        const std::string filename =
            args.outputFileName.empty()
                ? "aa_mirror_scene.png"
                : args.outputFileName;

        image.write(filename);

        std::cout
            << "Wrote " << filename
            << " (" << width << "x" << height
            << ", " << args.rpp << " rays/pixel"
            << ", depth " << args.recursionDepth
            << ", requested CPUs " << args.numCpus
            << ")\n";

#ifndef _OPENMP
        std::cout
            << "OpenMP was not available; "
            << "rendering ran serially.\n";
#endif

        return 0;

    } catch (const std::exception& e) {

        std::cerr
            << "Render error: "
            << e.what()
            << '\n';

        return 1;
    }
}
