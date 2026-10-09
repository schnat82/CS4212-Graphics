# 
# Adds the src directory to the include search
# path. ${CMAKE_SOURCE_DIR} is prefixed to allow the files in the
# utest directory to correctly include the relative reference to the
# src files.
#

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(
    test_pngWrite
    test_pngWrite.cpp
)

target_link_libraries(
    test_pngWrite
    cs4212-util
    ${Boost_PROGRAM_OPTIONS_LIBRARIES}
    PNG::PNG
    ZLIB::ZLIB
)

add_executable(
    test_readPNGFile
    test_readPNGFile.cpp
)

target_link_libraries(
    test_readPNGFile
    cs4212-util
    ${Boost_PROGRAM_OPTIONS_LIBRARIES}
    PNG::PNG
    ZLIB::ZLIB
)

foreach(example
    rainbow_gradient
    ray_direction
    sphere_flag
    multiple_shapes
    shaded_spheres
    prism_rainbow
    shadow_scene
)
    add_executable(
        ${example}
        ${example}.cpp
    )

    target_link_libraries(
        ${example}
        PRIVATE
        cs4212-util
        PNG::PNG
        ZLIB::ZLIB
    )
endforeach()

# New anti-aliasing and mirror renderer.
add_executable(
    aa_mirror_scene
    aa_mirror_scene.cpp
)

target_link_libraries(
    aa_mirror_scene
    PRIVATE
    cs4212-util
    PNG::PNG
    ZLIB::ZLIB
    Boost::program_options
)

# Enable parallel row rendering when OpenMP exists.
if(OpenMP_CXX_FOUND)
    target_link_libraries(
        aa_mirror_scene
        PRIVATE
        OpenMP::OpenMP_CXX
    )
endif()
