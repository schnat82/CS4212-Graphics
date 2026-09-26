# 
# Adds the src directory to the include search
# path. ${CMAKE_SOURCE_DIR} is prefixed to allow the files in the
# utest directory to correctly include the relative reference to the
# src files.
#
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(test_pngWrite
  test_pngWrite.cpp
)
target_link_libraries(test_pngWrite cs4212-util)
target_link_libraries(test_pngWrite ${Boost_PROGRAM_OPTIONS_LIBRARIES})
target_link_libraries(test_pngWrite PNG::PNG)
target_link_libraries(test_pngWrite ZLIB::ZLIB)

add_executable(test_readPNGFile
  test_readPNGFile.cpp
)
target_link_libraries(test_readPNGFile cs4212-util)
target_link_libraries(test_readPNGFile ${Boost_PROGRAM_OPTIONS_LIBRARIES})
target_link_libraries(test_readPNGFile PNG::PNG)
target_link_libraries(test_readPNGFile ZLIB::ZLIB)

add_executable(rainbow_gradient
  rainbow_gradient.cpp
)
target_link_libraries(rainbow_gradient cs4212-util)
target_link_libraries(rainbow_gradient PNG::PNG)
target_link_libraries(rainbow_gradient ZLIB::ZLIB)

add_executable(ray_direction
  ray_direction.cpp
)
target_link_libraries(ray_direction cs4212-util)
target_link_libraries(ray_direction PNG::PNG)
target_link_libraries(ray_direction ZLIB::ZLIB)

add_executable(sphere_flag
  sphere_flag.cpp
)
target_link_libraries(sphere_flag cs4212-util)
target_link_libraries(sphere_flag PNG::PNG)
target_link_libraries(sphere_flag ZLIB::ZLIB)


add_executable(multiple_shapes
  multiple_shapes.cpp
)

target_link_libraries(multiple_shapes cs4212-util)
target_link_libraries(multiple_shapes PNG::PNG)
target_link_libraries(multiple_shapes ZLIB::ZLIB)