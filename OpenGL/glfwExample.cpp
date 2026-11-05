#include <cstdlib>
#include <iostream>
#include <vector>
#include <cstddef>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"
#include "OpenGLCamera.h"

struct Vertex {
    GLfloat x, y, z;
    GLfloat r, g, b;
};

int main(void)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        1000, 1000, "CS4212 - Model View Projection Matrix Lab", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: "
                  << glewGetErrorString(err) << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glGetError();

    std::cout << "\n========== OPENGL INFORMATION ==========\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    int glfwMajor = 0, glfwMinor = 0, glfwRevision = 0;
    glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);
    std::cout << "GLFW version: " << glfwMajor << '.' << glfwMinor << '.' << glfwRevision << '\n';
    GLint majorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    std::cout << "GL_MAJOR_VERSION: " << majorVersion << '\n';
    std::cout << "========================================\n";

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Keep the original lab's dark blue-gray background.
    glClearColor(0.14f, 0.17f, 0.22f, 1.0f);

    // The triangle now lives in model/world units instead of clip coordinates.
    std::vector<Vertex> host_VertexBuffer = {
        {-3.0f, -3.0f, 0.0f, 1.00f, 0.40f, 0.15f},  // orange
        { 3.0f, -3.0f, 0.0f, 0.10f, 0.95f, 0.75f},  // turquoise
        { 0.0f,  3.0f, 0.0f, 0.60f, 0.25f, 1.00f}   // purple
    };

    GLuint m_triangleVBO = 0;
    glGenBuffers(1, &m_triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(host_VertexBuffer.size() * sizeof(Vertex)),
                 host_VertexBuffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    host_VertexBuffer.clear();

    GLuint m_VAO = 0;
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, x)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, r)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Professor's existing GLSLObject wrapper loads shader files relative to
    // the working directory. CMake places the files beside the executable.
    sivelab::GLSLObject shader;
    shader.addShader("vertexShader_withMatrixTransformation.glsl",
                     sivelab::GLSLObject::VERTEX_SHADER);
    shader.addShader("fragmentShader_passthrough.glsl",
                     sivelab::GLSLObject::FRAGMENT_SHADER);
    if (shader.createProgram() == 0) {
        std::cerr << "Failed to link shader program!\n";
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_triangleVBO);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    const GLint projMatrixID  = shader.createUniform("projMatrix");
    const GLint viewMatrixID  = shader.createUniform("viewMatrix");
    const GLint modelMatrixID = shader.createUniform("modelMatrix");
    const GLint useVertexColorsLocation = shader.createUniform("useVertexColors");
    if (projMatrixID < 0 || viewMatrixID < 0 || modelMatrixID < 0) {
        std::cerr << "Required matrix uniform was not found.\n";
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_triangleVBO);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Both projections use the same camera; projection switching does not jump
    // to a different world location. At z=0, the orthographic view sees an
    // upright 6-unit triangle from the origin. Perspective starts at z=10.
    OpenGLCamera camera(glm::vec3(0.0f, 0.0f, 10.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f));

    bool useVertexColors = true;
    bool usePerspective = false;
    bool animateModel = true;
    float modelAngle = 0.0f;
    float modelScale = 1.0f;
    float modelOffsetX = 0.0f;
    float modelOffsetY = 0.0f;
    bool prevP = false, prevO = false, prevSpace = false, prevR = false;
    double lastFrameTime = glfwGetTime();
    double lastFpsPrint = lastFrameTime;
    int framesSincePrint = 0;

    std::cout << "\nCONTROLS:\n"
              << "1 / 2    Solid / interpolated vertex colors\n"
              << "O / P    Orthographic / perspective projection\n"
              << "W A S D  Move camera forward / left / backward / right\n"
              << "Q / E    Move camera down / up\n"
              << "Arrow keys  Translate the triangle\n"
              << "Z / X    Shrink / enlarge the triangle\n"
              << "SPACE    Pause / resume triangle rotation\n"
              << "R        Reset camera and model transformation\n"
              << "T        Print rolling FPS once per second while held\n"
              << "ESC      Exit\n\n";

    while (!glfwWindowShouldClose(window)) {
        const double now = glfwGetTime();
        float deltaTime = static_cast<float>(now - lastFrameTime);
        lastFrameTime = now;
        // Prevent giant jumps after a debugger break or window drag.
        deltaTime = glm::clamp(deltaTime, 0.0f, 0.10f);

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            useVertexColors = false;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            useVertexColors = true;

        const bool pDown = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
        const bool oDown = glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS;
        const bool spaceDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        const bool rDown = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
        if (pDown && !prevP) usePerspective = true;
        if (oDown && !prevO) usePerspective = false;
        if (spaceDown && !prevSpace) animateModel = !animateModel;
        if (rDown && !prevR) {
            camera = OpenGLCamera(glm::vec3(0.0f, 0.0f, 10.0f),
                                  glm::vec3(0.0f, 0.0f, -1.0f));
            modelAngle = 0.0f;
            modelScale = 1.0f;
            modelOffsetX = 0.0f;
            modelOffsetY = 0.0f;
            animateModel = true;
        }
        prevP = pDown;
        prevO = oDown;
        prevSpace = spaceDown;
        prevR = rDown;

        const float cameraStep = 5.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.moveForward(cameraStep);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.moveForward(-cameraStep);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.moveRight(-cameraStep);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.moveRight(cameraStep);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.moveUp(-cameraStep);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.moveUp(cameraStep);

        const float transformStep = 3.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            modelOffsetX -= transformStep;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            modelOffsetX += transformStep;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            modelOffsetY -= transformStep;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            modelOffsetY += transformStep;
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            modelScale = glm::max(0.1f, modelScale - deltaTime);
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            modelScale = glm::min(4.0f, modelScale + deltaTime);
        if (animateModel) modelAngle += glm::radians(35.0f) * deltaTime;

        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        if (fbWidth == 0 || fbHeight == 0) continue;
        glViewport(0, 0, fbWidth, fbHeight);
        const float aspect = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);

        const glm::mat4 M_view = camera.viewMatrix();
        const glm::mat4 M_proj = usePerspective
            ? camera.perspectiveMatrix(aspect)
            : camera.orthographicMatrix(aspect);

        // Model transform: translate, then rotate, then scale; GLM operates on
        // column vectors, so the resulting rightmost operation occurs first.
        glm::mat4 M_model(1.0f);
        M_model = glm::translate(M_model, glm::vec3(modelOffsetX, modelOffsetY, 0.0f));
        M_model = glm::rotate(M_model, modelAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        M_model = glm::scale(M_model, glm::vec3(modelScale));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.activate();
        glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(M_proj));
        glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr(M_view));
        glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(M_model));
        if (useVertexColorsLocation >= 0)
            glUniform1i(useVertexColorsLocation, useVertexColors ? 1 : 0);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        shader.deactivate();
        glfwSwapBuffers(window);

        ++framesSincePrint;
        if (now - lastFpsPrint >= 1.0) {
            if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
                std::cout << "FPS: " << framesSincePrint / (now - lastFpsPrint)
                          << " | Projection: " << (usePerspective ? "Perspective" : "Ortho")
                          << " | Camera: (" << camera.position().x << ", "
                          << camera.position().y << ", " << camera.position().z
                          << ")\n";
            }
            framesSincePrint = 0;
            lastFpsPrint = now;
        }
    }

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_triangleVBO);
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
