
add_library(cs4212-util
    ArgumentParsing.cpp
    ArgumentParsing.h

    handleGraphicsArgs.cpp
    handleGraphicsArgs.h

    model_obj.cpp
    model_obj.h

    ray.cpp
    ray.h

    framebuffer.cpp
    framebuffer.h

    camera.cpp
    camera.h

    shape.h
    shader.h

    sphere.cpp
    sphere.h

    triangle.cpp
    triangle.h

    lambertian_shader.cpp
    lambertian_shader.h

    blinn_phong_shader.cpp
    blinn_phong_shader.h

    mirror_shader.cpp
    mirror_shader.h

    scene.cpp
    scene.h
)

target_compile_definitions(
    cs4212-util
    PUBLIC HAS_GLM
)

target_link_libraries(
    cs4212-util
    PRIVATE Boost::program_options
)

target_link_libraries(
    cs4212-util
    PUBLIC glm::glm
)
