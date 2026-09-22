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
    PerspectiveCamera(
        const point& position,
        const vec3& viewDirection,
        double imagePlaneWidth,
        double imagePlaneHeight,
        double focalLength,
        int imageWidth,
        int imageHeight
    );

    ray generateRay(int i, int j) override;

private:
    double planeWidth;
    double planeHeight;
    double focalLength;

    int imageWidth;
    int imageHeight;
};

#endif