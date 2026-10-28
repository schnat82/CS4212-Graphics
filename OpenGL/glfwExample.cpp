
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cstddef>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"

// Each vertex has 3 position floats and 3 color floats.
// Position: x, y, z
// Color:    r, g, b
struct Vertex {
    GLfloat x, y, z;
    GLfloat r, g, b;
};

int main(void)
{
    // STEP 1: Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return EXIT_FAILURE;
    }

    // Request an OpenGL 4.1 core profile context.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    const int winWidth = 1000;
    const int winHeight = 1000;

    GLFWwindow* window = glfwCreateWindow(
        winWidth,
        winHeight,
        "CS4212 - OpenGL Triangle Lab",
        nullptr,
        nullptr
    );

    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW after creating the OpenGL context.
    glewExperimental = GL_TRUE;

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: "
                  << glewGetErrorString(err) << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Clear any harmless error from GLEW initialization.
    glGetError();

    // STEP 1: Print graphics hardware information.
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* glslVersion =
        glGetString(GL_SHADING_LANGUAGE_VERSION);

    int glfwMajor, glfwMinor, glfwRevision;
    glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);

    std::cout << "\n========== OPENGL INFORMATION ==========\n";
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: "
              << version << std::endl;
    std::cout << "GPU Vendor: " << vendor << std::endl;
    std::cout << "GLSL Version: " << glslVersion << std::endl;
    std::cout << "GLFW Version: "
              << glfwMajor << "."
              << glfwMinor << "."
              << glfwRevision << std::endl;

    GLint majorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);

    std::cout << "GL_MAJOR_VERSION: "
              << majorVersion << std::endl;
    std::cout << "========================================\n";

    // Configure OpenGL.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Custom dark gray-blue background.
    glClearColor(0.14f, 0.17f, 0.22f, 1.0f);

    // STEP 2: Define our triangle vertices.
    //
    // Each vertex contains its position and its color.
    // The positions are already in normalized device space.
    //
    // Vertex 0 (bottom left): orange
    // Vertex 1 (bottom right): turquoise
    // Vertex 2 (top): purple

    std::vector<Vertex> host_VertexBuffer = {
        {-0.80f, -0.70f, 0.0f, 1.00f, 0.40f, 0.15f},
        { 0.80f, -0.70f, 0.0f, 0.10f, 0.95f, 0.75f},
        { 0.00f,  0.85f, 0.0f, 0.60f, 0.25f, 1.00f}
    };

    // STEP 2, PART 1: Create a Vertex Buffer Object.
    GLuint m_triangleVBO = 0;

    glGenBuffers(1, &m_triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO);

    // Copy vertex data from host (CPU) to device (GPU).
    GLsizeiptr numBytes = static_cast<GLsizeiptr>(
        host_VertexBuffer.size() * sizeof(Vertex)
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        numBytes,
        host_VertexBuffer.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vertex data is now stored on the GPU.
    host_VertexBuffer.clear();

    // STEP 2, PART 2: Create a Vertex Array Object.
    GLuint m_VAO = 0;

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO);

    // Attribute location 0: vertex position (x, y, z).
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, x))
    );

    // STEP 3: Attribute location 1: vertex color (r, g, b).
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, r))
    );

    // Unbind VAO and VBO.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // STEP 2, PART 3: Create our GLSL shader program.
    //
    // Uses the GLSLObject class already in your repository.
    sivelab::GLSLObject shader;

    shader.addShader(
        "vertexShader_passthrough.glsl",
        sivelab::GLSLObject::VERTEX_SHADER
    );

    shader.addShader(
        "fragmentShader_passthrough.glsl",
        sivelab::GLSLObject::FRAGMENT_SHADER
    );

    if (shader.createProgram() == 0) {
        std::cerr << "Failed to link shader program!"
                  << std::endl;

        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_triangleVBO);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Uniform used to switch between Step 2 and Step 3.
    GLint useVertexColorsLocation =
        shader.createUniform("useVertexColors");

    // Default to Step 3: interpolated vertex colors.
    bool useVertexColors = true;

    std::cout << "\nCONTROLS:\n";
    std::cout << "1 - Solid color triangle (Step 2)\n";
    std::cout << "2 - Interpolated vertex colors (Step 3)\n";
    std::cout << "ESC - Exit program\n\n";

    // STEP 2, PART 4: Main rendering loop.
    while (!glfwWindowShouldClose(window))
    {
        // Handle keyboard controls.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            useVertexColors = false;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            useVertexColors = true;
        }

        // Update viewport to match framebuffer size.
        int fbWidth = 0;
        int fbHeight = 0;

        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);

        // Clear screen using our custom background color.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader program.
        shader.activate();

        // Tell fragment shader which rendering mode to use.
        if (useVertexColorsLocation >= 0) {
            glUniform1i(
                useVertexColorsLocation,
                useVertexColors ? 1 : 0
            );
        }

        // Bind the triangle VAO.
        glBindVertexArray(m_VAO);

        // Render one triangle containing three vertices.
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Unbind the VAO.
        glBindVertexArray(0);

        // Deactivate shader.
        shader.deactivate();

        // Display rendered frame.
        glfwSwapBuffers(window);

        // Process window and keyboard events.
        glfwPollEvents();
    }

    // Clean up GPU resources.
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_triangleVBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
