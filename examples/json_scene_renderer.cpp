#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include "png++/png.hpp"
#include "SceneLoader.h"
#include "SceneParser_JSON.h"
#include "framebuffer.h"

int main(int argc, char** argv) {
    if (argc < 2 || argc > 6) {
        std::cerr << "Usage: json_scene_renderer <scene.json> [output.png] [width] [height] [samples-per-pixel]\n";
        return 1;
    }
    try {
        const std::string input = argv[1];
        const std::string output = argc > 2 ? argv[2] : "json_scene.png";
        const int width = argc > 3 ? std::stoi(argv[3]) : 500;
        const int height = argc > 4 ? std::stoi(argv[4]) : 500;
        const int samples = argc > 5 ? std::stoi(argv[5]) : 16;
        if (width <= 0 || height <= 0 || samples <= 0 || width > 8192 || height > 8192 || samples > 10000)
            throw std::invalid_argument("Invalid width, height or sample count");
        Scene scene;
        auto loader = std::make_shared<SceneLoader>(scene, width, height);
        SceneParser_JSON parser(loader);
        parser.parseFileData(input);
        auto& camera = loader->camera();
        if (!loader->shapeCount()) throw std::runtime_error("Scene has no supported shapes");
        std::cout << "Rendering " << loader->shapeCount() << " shapes and " << loader->lightCount() << " lights\n";
        Framebuffer framebuffer(width, height);
        const int n = int(std::sqrt(double(samples)));
        const bool stratified = n * n == samples;
        for (int y = 0; y < height; ++y) {
            std::mt19937 gen(12345u + unsigned(y));
            std::uniform_real_distribution<double> jitter(0.0, 1.0);
            for (int x = 0; x < width; ++x) {
                vec3 sum(0, 0, 0);
                for (int s = 0; s < samples; ++s) {
                    double dx = stratified ? ((s % n) + jitter(gen)) / n : jitter(gen);
                    double dy = stratified ? ((s / n) + jitter(gen)) / n : jitter(gen);
                    sum += scene.computeRayColor(camera.generateRay(x + dx, y + dy),
                        0.001, std::numeric_limits<double>::infinity(), 5);
                }
                framebuffer.setPixel(x, y, sum / double(samples));
            }
        }
        png::image<png::rgb_pixel> image(width, height);
        auto channel = [](double a) -> png::byte {
            return static_cast<png::byte>(std::clamp(a, 0.0, 1.0) * 255.0);
        };
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x) {
                const vec3 c = framebuffer.getPixel(x, y);
                image[y][x] = png::rgb_pixel(channel(c.x()), channel(c.y()), channel(c.z()));
            }
        image.write(output);
        std::cout << "Wrote " << output << " (" << width << "x" << height << ", " << samples << " spp)\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "JSON scene render error: " << e.what() << '\n';
        return 1;
    }
}
