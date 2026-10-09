
#include "camera.h"

Camera::Camera()
    : U(1, 0, 0),
      V(0, 1, 0),
      W(0, 0, 1),
      origin(0, 0, 0) {
}

PerspectiveCamera::PerspectiveCamera(
    const point& position,
    const vec3& viewDirection,
    double imagePlaneWidth,
    double imagePlaneHeight,
    double focal,
    int width,
    int height
)
    : planeWidth(imagePlaneWidth),
      planeHeight(imagePlaneHeight),
      focalLength(focal),
      imageWidth(width),
      imageHeight(height) {

    origin = position;

    W = unit_vector(viewDirection);

    const vec3 worldUp(0, 1, 0);

    U = unit_vector(cross(W, worldUp));
    V = unit_vector(cross(U, W));
}

ray PerspectiveCamera::generateRay(int i, int j) {
    return generateRay(
        static_cast<double>(i) + 0.5,
        static_cast<double>(j) + 0.5
    );
}

ray PerspectiveCamera::generateRay(double i, double j) {
    const double u = i / imageWidth - 0.5;
    const double v = 0.5 - j / imageHeight;

    const vec3 direction =
        focalLength * W
        + (u * planeWidth) * U
        + (v * planeHeight) * V;

    return ray(origin, direction);
}
