
# OpenGL Triangle Lab - CS4212

include_directories(${CMAKE_SOURCE_DIR}/src)

# Create the GLFW example executable.
add_executable(glfwExample
    glfwExample.cpp
    GLSL.cpp
    GLSL.h
)

# Link OpenGL dependencies.
target_link_libraries(glfwExample PRIVATE
    GLEW::GLEW
    glfw
)

# Platform-specific OpenGL linking.
if(APPLE)
    target_link_libraries(glfwExample PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
else()
    target_link_libraries(glfwExample PRIVATE
        OpenGL::GL
    )
endif()

# Preserve existing project dependencies.
target_link_libraries(glfwExample PRIVATE
    PNG::PNG
    ZLIB::ZLIB
)

# Shader files used by the OpenGL program.
set(SHADER_FILES
    vertexShader_passthrough.glsl
    fragmentShader_passthrough.glsl
)

# Automatically copy shaders beside the executable.
foreach(SHADER_FILE ${SHADER_FILES})
    add_custom_command(
        TARGET glfwExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}"
            "$<TARGET_FILE_DIR:glfwExample>/${SHADER_FILE}"
        COMMENT "Copying shader: ${SHADER_FILE}"
    )
endforeach()
