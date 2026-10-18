
#include <algorithm>
#include <cmath>
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
#include "normal_shader.h"
#include "diffuse_shader.h"
#include "handleGraphicsArgs.h"

#ifdef _OPENMP
#include <omp.h>
#endif

int main(int argc, char** argv) {
    try {
        sivelab::GraphicsArgs args;
        args.width = 500;
        args.height = 500;
        args.rpp = 100;
        args.recursionDepth = 5;
        args.process(argc, argv);

        if (args.width <= 0 || args.height <= 0 ||
            args.rpp <= 0 || args.recursionDepth <= 0 ||
            args.numCpus <= 0) {
            throw std::invalid_argument("Invalid render settings");
        }

        const int width = args.width;
        const int height = args.height;
        Framebuffer framebuffer(width, height);

        // Camera
        PerspectiveCamera camera(
            point(0, 3, 3),
            vec3(0, -0.4, -1),
            0.5,
            0.5,
            0.4,
            width,
            height
        );

        // Scene and lights
        const point light1(-5, 8, 5);
        const point light2(6, 6, 2);

        Scene scene(vec3(0.5, 0.7, 1.0), light1);
        scene.clearLights();
        scene.addLight(light1, vec3(0.7, 0.6, 0.45));
        scene.addLight(light2, vec3(0.35, 0.45, 0.7));

        // Materials
        auto grey = std::make_shared<LambertianShader>(
            vec3(0.6, 0.6, 0.6), light1);

        auto yellow = std::make_shared<LambertianShader>(
            vec3(0.9, 0.8, 0.1), light1);

        auto green = std::make_shared<LambertianShader>(
            vec3(0.2, 0.8, 0.2), light1);

        auto purple = std::make_shared<LambertianShader>(
            vec3(0.6, 0.2, 0.8), light1);

        auto red = std::make_shared<BlinnPhongShader>(
            vec3(0.8, 0.1, 0.1),
            vec3(1, 1, 1),
            64,
            light1
        );

        auto blue = std::make_shared<BlinnPhongShader>(
            vec3(0.1, 0.2, 0.8),
            vec3(1, 1, 1),
            16,
            light1
        );

        auto orange = std::make_shared<BlinnPhongShader>(
            vec3(0.95, 0.5, 0.1),
            vec3(1, 1, 1),
            32,
            light1
        );

        auto mirror = std::make_shared<MirrorShader>();
        auto normal = std::make_shared<NormalShader>();
        auto teal = std::make_shared<DiffuseShader>(
            vec3(0.2, 0.7, 0.7));

        // Helper functions for adding shapes
        const auto addTriangle = [&](
            const point& a,
            const point& b,
            const point& c,
            const std::shared_ptr<Shader>& material
        ) {
            auto triangle = std::make_shared<Triangle>(a, b, c);
            triangle->setShader(material);
            scene.add(triangle);
        };

        const auto addSphere = [&](
            const point& center,
            double radius,
            const std::shared_ptr<Shader>& material
        ) {
            auto sphere = std::make_shared<Sphere>(center, radius);
            sphere->setShader(material);
            scene.add(sphere);
        };

        // Ground
        addTriangle(
            point(-50, 0, 50),
            point(50, 0, 50),
            point(50, 0, -50),
            grey
        );

        addTriangle(
            point(-50, 0, 50),
            point(50, 0, -50),
            point(-50, 0, -50),
            grey
        );

        // Mirror wall
        addTriangle(
            point(-4, 0, -6),
            point(4, 0, -6),
            point(4, 4, -6),
            mirror
        );

        addTriangle(
            point(-4, 0, -6),
            point(4, 4, -6),
            point(-4, 4, -6),
            mirror
        );

        // Floating yellow triangle
        addTriangle(
            point(0.9, 1.4, -0.6),
            point(1.9, 1.4, -0.6),
            point(1.4, 2.2, -1.1),
            yellow
        );

        // Spheres
        addSphere(point(-2, 0.8, -3), 0.8, green);
        addSphere(point(-1.1, 0.6, -1.2), 0.6, red);
        addSphere(point(0, 0.9, -3.5), 0.9, normal);
        addSphere(point(0.3, 0.45, -1.6), 0.45, blue);
        addSphere(point(2, 1, -3), 1.0, mirror);
        addSphere(point(1.5, 0.5, 0.3), 0.5, mirror);
        addSphere(point(-1.3, 0.45, 0.4), 0.45, teal);

        // Sphere behind camera
        addSphere(point(0.3, 1, 5), 1.0, orange);

        // Purple sphere:
        // Add once its center and radius are provided.
        (void)purple;

        // Jittered anti-aliasing
        const int n = static_cast<int>(
            std::sqrt(static_cast<double>(args.rpp))
        );
        const bool stratified = n * n == args.rpp;

#ifdef _OPENMP
        omp_set_dynamic(0);
        omp_set_num_threads(args.numCpus);

#pragma omp parallel for schedule(dynamic, 1)
#endif
        for (int y = 0; y < height; ++y) {
            std::seed_seq seed{
                12345u,
                static_cast<unsigned>(y),
                static_cast<unsigned>(width),
                static_cast<unsigned>(height)
            };

            std::mt19937 generator(seed);
            std::uniform_real_distribution<double> jitter(0.0, 1.0);

            for (int x = 0; x < width; ++x) {
                vec3 accumulated(0, 0, 0);

                for (int sample = 0; sample < args.rpp; ++sample) {
                    const double dx = stratified
                        ? ((sample % n) + jitter(generator)) / n
                        : jitter(generator);

                    const double dy = stratified
                        ? ((sample / n) + jitter(generator)) / n
                        : jitter(generator);

                    const ray r = camera.generateRay(
                        x + dx, y + dy
                    );

                    accumulated += scene.computeRayColor(
                        r,
                        0.001,
                        std::numeric_limits<double>::infinity(),
                        args.recursionDepth
                    );
                }

                framebuffer.setPixel(
                    x, y, accumulated / args.rpp
                );
            }
        }

        // PNG output
        png::image<png::rgb_pixel> image(width, height);

        const auto channel = [](double value) -> png::byte {
            return static_cast<png::byte>(
                255.0 * std::clamp(value, 0.0, 1.0)
            );
        };

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const vec3 c = framebuffer.getPixel(x, y);

                image[y][x] = png::rgb_pixel(
                    channel(c.x()),
                    channel(c.y()),
                    channel(c.z())
                );
            }
        }

        const std::string filename = args.outputFileName.empty()
            ? "test_scene.png"
            : args.outputFileName;

        image.write(filename);

        std::cout << "Wrote " << filename
                  << " (" << width << "x" << height
                  << ", " << args.rpp << " rays/pixel"
                  << ", depth " << args.recursionDepth
                  << ")\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Render error: " << e.what() << '\n';
        return 1;
    }
}
