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

    // Camera forward direction.
    W = unit_vector(viewDirection);

    // World up vector.
    vec3 worldUp(0, 1, 0);

    // Right vector.
    U = unit_vector(cross(W, worldUp));

    // Camera up vector.
    V = unit_vector(cross(U, W));
}

ray PerspectiveCamera::generateRay(int i, int j) {

    // Convert pixel center into coordinates from -0.5 to +0.5.
    double u =
        (static_cast<double>(i) + 0.5) / imageWidth - 0.5;

    double v =
        0.5 - (static_cast<double>(j) + 0.5) / imageHeight;

    // Move from camera position to corresponding point on image plane.
    vec3 direction =
        focalLength * W
        + (u * planeWidth) * U
        + (v * planeHeight) * V;

    return ray(origin, direction);
}