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
    point origin;
};

class PerspectiveCamera : public Camera {
public:
    virtual ~PerspectiveCamera() = default;

    virtual ray generateRay(int i, int j) override = 0;
};

#endif