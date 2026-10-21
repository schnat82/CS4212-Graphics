#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <memory>
#include <string>

#include "SceneParser_JSON.h"

class Mock_SceneLoader : public ISceneLoader {
public:
  Mock_SceneLoader()
      : numCameras(0), numLights(0), numShapes(0), numTextures(0) {}

  void reserveCameras(size_t count) override { numCameras = count; }
  void reserveLights(size_t count) override { numLights = count; }
  void reserveShapes(size_t count) override { numShapes = count; }
  void reserveShaders(size_t count) override { numShaders = count; }
  void reserveTextures(size_t count) override { numTextures = count; }

  void setSceneParams(const SceneParams &sparams) override {
    std::cout << "Setting scene params." << std::endl;
  }

  void addCamera(const std::string &name, const std::string &type,
                 const ISceneLoader::vec &pos, const ISceneLoader::vec &viewDir,
                 float focalLength, float imagePlaneWidth) override {
    std::cout << "Creating camera: " << name << ", type:" << type << std::endl;
  }
  void addPointLight(const ISceneLoader::vec &pos,
                     const ISceneLoader::vec &intensity) override {
    std::cout << "Creating point light." << std::endl;
  }
  void addAreaLight(const ISceneLoader::vec &pos,
                    const ISceneLoader::vec &color,
                    const ISceneLoader::vec &normal, float width,
                    float length) override {
    std::cout << "Creating area light." << std::endl;
  }
  void addShader(const ISceneLoader::ShaderDesc &shaderDesc) override {
    std::cout << "Creating shader: type=" << shaderDesc.type << std::endl;
  }
  void addShape(const ISceneLoader::ShapeDesc &shapeDesc) override {
    std::cout << "Creating shape: type=" << shapeDesc.type << std::endl;
  }
  void addTexture(const std::string &type, const std::string &name,
                  const std::string &sourceFile) override {
    std::cout << "Creating texture: type=" << type << ", name=" << name
              << ", sourceFile=" << sourceFile << std::endl;
  }

public:
  int numCameras;
  int numLights;
  int numShapes;
  int numShaders;
  int numTextures;
};

TEST_CASE("Camera JSON Parsing") {
  std::string datasource =
      "{\"camera\": [{ \"position\": \"0 3.0 4.0\", \"viewDir\": \"0 -1.5 "
      "-3.0\", \"focalLength\": 0.40, \"imagePlaneWidth\": 0.5, \"_name\": "
      "\"main\", \"_type\": \"perspective\"}, { \"position\": \"0 3.0 4.0\", "
      "\"viewDir\": \"0 -1.5 -3.0\", \"focalLength\": 0.40, "
      "\"imagePlaneWidth\": 0.5, \"_name\": \"main\", \"_type\": "
      "\"perspective\"}, { \"position\": [0, 3.0, 4.0], \"viewDir\": [0, -1.5, "
      "-3.0], \"focalLength\": 0.40, \"imagePlaneWidth\": 0.5, \"_name\": "
      "\"main\", \"_type\": \"perspective\"}]}";

  std::shared_ptr<Mock_SceneLoader> sload =
      std::make_shared<Mock_SceneLoader>();
  SceneParser_JSON sceneParser(sload);

  sceneParser.parseStringData(datasource);
  REQUIRE(sload->numCameras == 3);
}

TEST_CASE("Light JSON Parsing") {
  std::string datasource =
      "{\"light\": [{ \"position\": [0.0, 4.0, 0.0], \"intensity\": [0.0, 1.0, "
      "0.0], \"_type\": \"point\" }, { \"position\": \"0.0 0.0 4.0\", "
      "\"intensity\": \"0.0 0.0 1.0\", \"_type\": \"point\" }, {\"position\": "
      "\"4.0 0.0 0.0\", \"intensity\": \"1.0 0.0 0.0\", \"_type\": \"point\" "
      "}, { \"position\": \"4.0 4.0 0.0\", \"intensity\": \"1.0 1.0 0.0\", "
      "\"_type\": \"point\" },{\"position\": \"0.0 4.0 4.0\",\"intensity\": "
      "\"0.0 1.0 1.0\",\"_type\": \"point\" }, {\"position\": \"4.0 0.0 "
      "4.0\",\"intensity\": \"1.0 0.0 1.0\",\"_type\": \"point\"}]}";

  std::shared_ptr<Mock_SceneLoader> sload =
      std::make_shared<Mock_SceneLoader>();
  SceneParser_JSON sceneParser(sload);

  sceneParser.parseStringData(datasource);
  REQUIRE(sload->numLights == 6);
}

TEST_CASE("Shader JSON Parsing") {
  std::string datasource =
      "{\"shader\": [{\"diffuse\": [0.941819, 0.484835, "
      "0.670388],\"specular\": \"1 1 1\",\"phongExp\": 35.5465,\"_name\": "
      "\"blinnphongMirror_0\",\"_type\": \"BlinnPhong\"},{\"diffuse\": "
      "\"0.185338 0.365224 0.873936\",\"_name\": \"lambertian_1\",\"_type\": "
      "\"Lambertian\"},{\"diffuse\": \"0.69342 0.890681 "
      "0.827891\",\"specular\": \"1 1 1\",\"phongExp\": 115.582,\"_name\": "
      "\"blinnphongMirror_2\",\"_type\": \"BlinnPhong\"},{\"diffuse\": "
      "\"0.99931 0.365784 0.136554\",\"specular\": \"1 1 1\",\"phongExp\": "
      "39.2122,\"_name\": \"blinnphong_3\",\"_type\": "
      "\"BlinnPhong\"},{\"diffuse\": \"0.512096 0.790364 0.438989\",\"_name\": "
      "\"lambertian_4\",\"_type\": \"Lambertian\"},{\"diffuse\": \"0.471322 "
      "0.594052 0.706368\",\"specular\": \"1 1 1\",\"phongExp\": "
      "101.984,\"_name\": \"blinnphongMirror_5\",\"_type\": "
      "\"BlinnPhong\"},{\"diffuse\": \"0.718401 0.817172 0.669762\",\"_name\": "
      "\"lambertian_6\",\"_type\": \"Lambertian\"},{\"diffuse\": \"0.886019 "
      "0.313354 0.930921\",\"specular\": \"1 1 1\",\"phongExp\": "
      "111.757,\"_name\": \"blinnphong_7\",\"_type\": "
      "\"BlinnPhong\"},{\"diffuse\": \"0.897668 0.244369 "
      "0.48263\",\"specular\": \"1 1 1\",\"phongExp\": 86.0139,\"_name\": "
      "\"blinnphongMirror_8\",\"_type\": \"BlinnPhong\"},{\"diffuse\": "
      "\"0.90733 0.247535 0.877337\",\"_name\": \"lambertian_9\",\"_type\": "
      "\"Lambertian\"},{\"diffuse\": \"1.0 1.0 1.0\",\"_name\": "
      "\"baseShader\",\"_type\": \"Lambertian\"}]}";

  std::shared_ptr<Mock_SceneLoader> sload =
      std::make_shared<Mock_SceneLoader>();
  SceneParser_JSON sceneParser(sload);

  sceneParser.parseStringData(datasource);
  REQUIRE(sload->numShaders == 11);
}

TEST_CASE("Shape JSON Parsing") {
  std::string datasource =
      R"( {"shape": [ {"shader": {"_ref": "white_chalk" }, "center": "-1.5 -1.5 -5", "radius": 1, "_name": "sphere1", "_type": "sphere" }, {"shader": {"_ref": "red_chalk"}, "center": "1.5 -1.5 -5", "radius": 1,"_name": "sphere2", "_type": "sphere" }, { "shader": { "_ref": "yellow_chalk" }, "center": "1.5 1.5 -5", "radius": 1, "_name": "sphere3", "_type": "sphere" }, {  "shader": {  "_ref": "blue_chalk"  }, "center": "-1.5 1.5 -5", "radius": 1, "_name": "sphere4", "_type": "sphere" } ]})";

  std::shared_ptr<Mock_SceneLoader> sload =
      std::make_shared<Mock_SceneLoader>();
  SceneParser_JSON sceneParser(sload);

  sceneParser.parseStringData(datasource);
  REQUIRE(sload->numShapes == 4);
}

TEST_CASE("Texture JSON Parsing") {
  std::string datasource =
      R"({"texture": [ { "sourcefile": "images/textureCalibrationNumberGrid.png", "name": "calibrationTex", "_type": "image" }, { "sourcefile": "images/moon_2k.png", "_name": "MoonDiffuse", "_type": "image" } ]})";

  std::shared_ptr<Mock_SceneLoader> sload =
      std::make_shared<Mock_SceneLoader>();
  SceneParser_JSON sceneParser(sload);

  sceneParser.parseStringData(datasource);
  REQUIRE(sload->numTextures == 2);
}

TEST_CASE("Scene Test 00") {
  std::string datasource =
      R"({"scene": { "sceneParameters": {"bgColor": "0.8 0.8 1.0", "envMapVertCross": "images/uffizi_cross.png"}, "camera": [ {"position": "0 0 0","viewDir": "0 0 -1","focalLength": 1.0,"imagePlaneWidth": 0.5,"_name": "main","_type": "perspective"} ],"light": [{"position": "-1 1 0","intensity": "1.0 1.0 1.0","_type": "point"}],"shader": [{"diffuse": "1 0 0","_name": "red","_type": "Lambertian" }, {"diffuse": "0 1 0","_name": "green","_type": "Lambertian" }, {"diffuse": "0 0 1","_name": "blue","_type": "Lambertian" }],"shape": [ {"shader": {"_ref": "red"},"v0": "-1.2 -0.2 -7","v1": "0.8 -0.5 -5","v2": "0.9 0 -5","_name": "triangle1","_type": "triangle" }, {"shader": {"_ref": "green"},"v0": "0.773205 -0.93923 -7","v1": "0.0330127 0.94282 -5","v2": "-0.45 0.779423 -5","_name": "triangle2","_type": "triangle" }, {"shader": {"_ref": "blue"},"v0": "0.426795 1.13923 -7","v1": "-0.833013 -0.44282 -5","v2": "-0.45 -0.779423 -5","_name": "triangle3","_type": "triangle" }]}})";

  std::shared_ptr<Mock_SceneLoader> sload =
      std::make_shared<Mock_SceneLoader>();
  SceneParser_JSON sceneParser(sload);

  sceneParser.parseStringData(datasource);
}
