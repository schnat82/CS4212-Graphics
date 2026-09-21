#include "camera.h"

Camera::Camera()
    : U(1, 0, 0),
      V(0, 1, 0),
      W(0, 0, 1),
      origin(0, 0, 0) {
}