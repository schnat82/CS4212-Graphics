#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Camera for GPU rasterization, separate from the ray-tracing Camera class.
// OpenGL convention: camera looks along its local -Z axis.
class OpenGLCamera {
public:
    OpenGLCamera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
                 const glm::vec3& direction = glm::vec3(0.0f, 0.0f, -1.0f))
        : position_(position), forward_(glm::normalize(direction)) {
        updateBasis();
    }

    glm::mat4 viewMatrix() const {
        return glm::lookAt(position_, position_ + forward_, up_);
    }

    glm::mat4 orthographicMatrix(float aspect) const {
        // A square 15-unit-wide volume at aspect=1, matching the lab.
        const float halfHeight = 7.5f;
        const float halfWidth = halfHeight * aspect;
        // Unlike the lab's unusual near=5, far=-5 example, use the
        // conventional positive near/far distances along the view direction.
        return glm::ortho(-halfWidth, halfWidth,
                          -halfHeight, halfHeight, 0.1f, 100.0f);
    }

    glm::mat4 perspectiveMatrix(float aspect) const {
        return glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
    }

    void moveForward(float distance) { position_ += forward_ * distance; }
    void moveRight(float distance) { position_ += right_ * distance; }
    void moveUp(float distance) { position_ += up_ * distance; }

    const glm::vec3& position() const { return position_; }
    const glm::vec3& forward() const { return forward_; }
    const glm::vec3& right() const { return right_; }
    const glm::vec3& up() const { return up_; }

private:
    void updateBasis() {
        // Use a fallback up axis if looking nearly parallel to world up.
        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        if (glm::abs(glm::dot(forward_, worldUp)) > 0.999f)
            worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
        right_ = glm::normalize(glm::cross(forward_, worldUp));
        up_ = glm::normalize(glm::cross(right_, forward_));
    }

    glm::vec3 position_;
    glm::vec3 forward_;
    glm::vec3 right_;
    glm::vec3 up_;
};
