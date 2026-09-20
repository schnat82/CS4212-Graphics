#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

class Camera {
public:
    Camera();
    virtual ~Camera() = default;

    virtual ray generateRay(int i, int j) = 0;

protected:
    vec3 U, V, W;
    vec3 origin;
};

class PerspectiveCamera : public Camera {
public:
    virtual ~PerspectiveCamera() = default;

    virtual ray generateRay(int i, int j) override = 0;
};


Camera::Camera()
    : origin(point(0, 0, 0)), U(vec3(1, 0, 0)), V(vec3(0, 1, 0)), W(vec3(0, 0, 1)) {
}
//Camera * cameraPtr = new PerspectiveCamera();

#endif