# OpenGL triangle with model, view and projection matrices.
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(glfwExample
    glfwExample.cpp
    GLSL.cpp
    GLSL.h
    OpenGLCamera.h
)

target_link_libraries(glfwExample PRIVATE
    GLEW::GLEW
    glfw
    glm::glm
)

if(APPLE)
    target_link_libraries(glfwExample PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
else()
    target_link_libraries(glfwExample PRIVATE OpenGL::GL)
endif()

target_link_libraries(glfwExample PRIVATE PNG::PNG ZLIB::ZLIB)

set(SHADER_FILES
    vertexShader_passthrough.glsl
    vertexShader_withMatrixTransformation.glsl
    fragmentShader_passthrough.glsl
)

foreach(SHADER_FILE ${SHADER_FILES})
    add_custom_command(
        TARGET glfwExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}"
            "$<TARGET_FILE_DIR:glfwExample>/${SHADER_FILE}"
        COMMENT "Copying shader: ${SHADER_FILE}"
    )
endforeach()