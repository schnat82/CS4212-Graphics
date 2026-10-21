#pragma once
#include "ISceneLoader.h"
#include "scene.h"
#include "camera.h"
#include "sphere.h"
#include "triangle.h"
#include "lambertian_shader.h"
#include "blinn_phong_shader.h"
#include "mirror_shader.h"
#include "diffuse_shader.h"
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cctype>

// Adapts the professor's parser callbacks to the existing CS4212 renderer.
class SceneLoader final : public ISceneLoader {
public:
    SceneLoader(Scene& scene, int width, int height)
      : scene_(scene), width_(width), height_(height) { scene_.clearLights(); }

    void reserveCameras(size_t) override {}
    void reserveLights(size_t) override {}
    void reserveShapes(size_t) override {}
    void reserveShaders(size_t count) override { shaders_.reserve(count); }
    void reserveTextures(size_t) override {}

    void setSceneParams(const SceneParams& p) override {
        if (!p.envMapFile.empty()) unsupported("environment map", p.envMapFile);
    }
    void addCamera(const std::string& name, const std::string& type,
                   const vec& pos, const vec& dir, float focal,
                   float planeWidth) override {
        if (lower(type) != "perspective") { unsupported("camera", type); return; }
        if (camera_) throw std::runtime_error("Multiple cameras are not supported");
        camera_ = std::make_unique<PerspectiveCamera>(toVec(pos), toVec(dir),
            planeWidth, planeWidth * double(height_) / width_, focal, width_, height_);
        std::cout << "Loaded camera: " << name << '\n';
    }
    void addPointLight(const vec& pos, const vec& intensity) override {
        scene_.addLight(toVec(pos), toVec(intensity));
        ++lights_;
    }
    void addAreaLight(const vec&, const vec&, const vec&, float, float) override {
        unsupported("light", "area");
    }
    void addShader(const ShaderDesc& d) override {
        std::shared_ptr<Shader> shader;
        const auto type = lower(d.type);
        if (type == "lambertian") shader = std::make_shared<LambertianShader>(toVec(d.diffuse.data));
        else if (type == "blinnphong" || type == "phong") shader =
            std::make_shared<BlinnPhongShader>(toVec(d.diffuse.data), toVec(d.specular.data), d.phongExp);
        else if (type == "mirror") shader = std::make_shared<MirrorShader>();
        else if (type == "diffuse") shader = std::make_shared<DiffuseShader>(toVec(d.diffuse.data));
        else { unsupported("shader", d.type); return; }
        if (d.diffuse.hasTexture || d.specular.hasTexture) unsupported("shader textures", d.name);
        shaders_[d.name] = std::move(shader);
        std::cout << "Loaded shader: " << d.name << " (" << d.type << ")\n";
    }
    void addShape(const ShapeDesc& d) override {
        if (d.hasXForm) {
            const glm::mat4 identity(1.0f);
            for (int c = 0; c < 4; ++c)
                for (int r = 0; r < 4; ++r)
                    if (std::abs(d.localXForm[c][r] - identity[c][r]) > 1e-5f)
                        throw std::runtime_error("Transformed shapes are not supported: " + d.name);
        }
        auto it = shaders_.find(d.shaderNameReference);
        if (it == shaders_.end())
            throw std::runtime_error("Unknown/unsupported shader reference '" + d.shaderNameReference + "' on shape '" + d.name + "'");
        std::shared_ptr<Shape> object;
        const auto type = lower(d.type);
        if (type == "sphere") {
            if (d.radius <= 0) throw std::runtime_error("Invalid sphere radius: " + d.name);
            object = std::make_shared<Sphere>(toVec(d.center), d.radius);
        } else if (type == "triangle") {
            object = std::make_shared<Triangle>(toVec(d.v0), toVec(d.v1), toVec(d.v2));
        } else { unsupported("shape", d.type); return; }
        object->setShader(it->second);
        scene_.add(object);
        ++shapes_;
        std::cout << "Loaded shape: " << d.name << " (" << d.type << ")\n";
    }
    void addTexture(const std::string& type, const std::string& name,
                    const std::string&) override { unsupported("texture", name + " (" + type + ")"); }

    PerspectiveCamera& camera() const {
        if (!camera_) throw std::runtime_error("No supported perspective camera found in JSON scene");
        return *camera_;
    }
    size_t shapeCount() const { return shapes_; }
    size_t lightCount() const { return lights_; }
private:
    static vec3 toVec(const vec& v) { return vec3(v.x, v.y, v.z); }
    static std::string lower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return char(std::tolower(c)); });
        return s;
    }
    static void unsupported(const std::string& kind, const std::string& name) {
        std::cerr << "WARNING: Unsupported " << kind << ": " << name << '\n';
    }
    Scene& scene_;
    int width_, height_;
    std::unique_ptr<PerspectiveCamera> camera_;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;
    size_t shapes_ = 0, lights_ = 0;
};
