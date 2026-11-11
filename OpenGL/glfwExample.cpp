#include <cstdlib>
#include <iostream>
#include <vector>
#include <cstddef>
#include <cmath>
#include <array>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"
#include "OpenGLCamera.h"

struct Vertex {
    GLfloat x, y, z;
    GLfloat nx, ny, nz;
};

struct ShaderProgram {
    sivelab::GLSLObject shader;
    GLint proj = -1, view = -1, model = -1, normal = -1;
    GLint light = -1, eye = -1, diffuse = -1, specular = -1, exponent = -1;

    bool load(const char* fragmentFilename) {
        shader.addShader("vertexShader_PrepForPerFragment.glsl",
                         sivelab::GLSLObject::VERTEX_SHADER);
        shader.addShader(fragmentFilename, sivelab::GLSLObject::FRAGMENT_SHADER);
        if (shader.createProgram() == 0) return false;
        proj = shader.createUniform("projMatrix");
        view = shader.createUniform("viewMatrix");
        model = shader.createUniform("modelMatrix");
        normal = shader.createUniform("normalMatrix");
        light = shader.createUniform("lightPosWorld");
        eye = shader.createUniform("eyePosWorld");
        diffuse = shader.createUniform("diffuseComponent");
        specular = shader.createUniform("specularComponent");
        exponent = shader.createUniform("phongExponent");
        return proj >= 0 && view >= 0 && model >= 0 && normal >= 0;
    }

    void setFrame(const glm::mat4& p, const glm::mat4& v,
                  const glm::vec3& eyePos, const glm::vec4& lightPos) {
        shader.activate();
        glUniformMatrix4fv(proj, 1, GL_FALSE, glm::value_ptr(p));
        glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(v));
        if (light >= 0) glUniform4fv(light, 1, glm::value_ptr(lightPos));
        if (eye >= 0) glUniform3fv(eye, 1, glm::value_ptr(eyePos));
    }

    void setObject(const glm::mat4& m, const glm::vec3& color) {
        const glm::mat4 n = glm::transpose(glm::inverse(m));
        glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(m));
        glUniformMatrix4fv(normal, 1, GL_FALSE, glm::value_ptr(n));
        if (diffuse >= 0) glUniform3fv(diffuse, 1, glm::value_ptr(color));
        if (specular >= 0) glUniform3f(specular, 0.95f, 0.95f, 0.95f);
        if (exponent >= 0) glUniform1f(exponent, 48.0f);
    }
};

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "CS4212 - GLSL Shaders Lab", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glGetError(); // GLEW may generate a harmless GL_INVALID_ENUM on core profiles.

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.14f, 0.17f, 0.22f, 1.0f);

    // Interleaved position + normal, replacing the earlier position + color VBO.
    // Counterclockwise triangle in the XY plane, facing toward +Z (the camera).
    const std::array<Vertex, 3> vertices = {{
        {-1.65f, -1.4f, 0.0f, 0.0f, 0.0f, 1.0f},
        { 1.65f, -1.4f, 0.0f, 0.0f, 0.0f, 1.0f},
        { 0.0f,   1.6f, 0.0f, 0.0f, 0.0f, 1.0f}
    }};

    GLuint vbo = 0, vao = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, x)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, nx)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::array<ShaderProgram, 3> shaders;
    const std::array<const char*, 3> fragmentFiles = {{
        "fragmentShader_Lambertian.glsl",
        "fragmentShader_BlinnPhong.glsl",
        "fragmentShader_Normal.glsl"
    }};
    for (std::size_t i = 0; i < shaders.size(); ++i) {
        if (!shaders[i].load(fragmentFiles[i])) {
            std::cerr << "Failed to load shaders for " << fragmentFiles[i] << '\n';
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
            glfwDestroyWindow(window);
            glfwTerminate();
            return EXIT_FAILURE;
        }
    }

    OpenGLCamera camera(glm::vec3(0.0f, 0.0f, 12.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f));
    int mode = 0;  // 0 Lambertian, 1 Blinn-Phong, 2 normal
    bool usePerspective = false;
    bool showTwoTriangles = true;
    bool animateModel = false;
    float angle = 0.0f;
    float modelScale = 1.0f;
    float offsetX = 0.0f, offsetY = 0.0f;
    double previousTime = glfwGetTime();
    bool prevSpace = false, prevR = false, prevB = false;

    std::cout << "\nCONTROLS:\n"
              << "1 Lambertian | 2 Blinn-Phong | 3 Normal visualization\n"
              << "B Toggle one/two triangles | O Orthographic | P Perspective\n"
              << "W/A/S/D move camera | Q/E move down/up\n"
              << "Arrow keys translate models | Z/X scale | SPACE rotate/pause\n"
              << "R Reset | ESC Exit\n";

    while (!glfwWindowShouldClose(window)) {
        const double now = glfwGetTime();
        const float dt = glm::clamp(static_cast<float>(now - previousTime), 0.0f, 0.1f);
        previousTime = now;
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) mode = 0;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) mode = 1;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) mode = 2;
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) usePerspective = false;
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) usePerspective = true;

        const bool space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        const bool reset = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
        const bool b = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
        if (space && !prevSpace) animateModel = !animateModel;
        if (b && !prevB) showTwoTriangles = !showTwoTriangles;
        if (reset && !prevR) {
            camera = OpenGLCamera(glm::vec3(0.0f, 0.0f, 12.0f),
                                  glm::vec3(0.0f, 0.0f, -1.0f));
            angle = 0.0f;
            modelScale = 1.0f;
            offsetX = offsetY = 0.0f;
            animateModel = false;
            usePerspective = false;
            showTwoTriangles = true;
        }
        prevSpace = space;
        prevR = reset;
        prevB = b;

        const float cameraStep = 5.0f * dt;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.moveForward(cameraStep);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.moveForward(-cameraStep);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.moveRight(-cameraStep);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.moveRight(cameraStep);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.moveUp(-cameraStep);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.moveUp(cameraStep);
        const float moveStep = 3.0f * dt;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) offsetX -= moveStep;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) offsetX += moveStep;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) offsetY -= moveStep;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) offsetY += moveStep;
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            modelScale = glm::max(0.1f, modelScale - dt);
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            modelScale = glm::min(4.0f, modelScale + dt);
        if (animateModel) angle += glm::radians(35.0f) * dt;

        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        if (width <= 0 || height <= 0) continue;
        glViewport(0, 0, width, height);
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        const glm::mat4 view = camera.viewMatrix();
        const glm::mat4 projection = usePerspective
            ? camera.perspectiveMatrix(aspect)
            : camera.orthographicMatrix(aspect);
        const glm::vec4 lightWorld(-3.0f, 5.0f, 10.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ShaderProgram& shader = shaders[static_cast<std::size_t>(mode)];
        shader.setFrame(projection, view, camera.position(), lightWorld);
        glBindVertexArray(vao);

        auto drawTriangle = [&](const glm::vec3& translation, const glm::vec3& color) {
            glm::mat4 model(1.0f);
            model = glm::translate(model, translation + glm::vec3(offsetX, offsetY, 0.0f));
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(modelScale));
            shader.setObject(model, color);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        };

        if (showTwoTriangles) {
            // Exact world-space positions requested by the lab.
            drawTriangle(glm::vec3(-2.0f, 1.0f, 4.0f), glm::vec3(0.12f, 0.35f, 1.0f));
            drawTriangle(glm::vec3( 2.0f,-1.0f, 4.0f), glm::vec3(1.0f, 0.12f, 0.12f));
        } else {
            drawTriangle(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.15f, 0.62f, 1.0f));
        }
        glBindVertexArray(0);
        shader.shader.deactivate();
        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
