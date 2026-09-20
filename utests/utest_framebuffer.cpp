#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "framebuffer.h"

TEST_CASE("Framebuffer stores width and height")
{
    Framebuffer framebuffer(800, 450);

    REQUIRE(framebuffer.width() == 800);
    REQUIRE(framebuffer.height() == 450);
}

TEST_CASE("Framebuffer starts with black pixels")
{
    Framebuffer framebuffer(800, 450);

    vec3 color = framebuffer.getPixel(0, 0);

    REQUIRE(color.x() == 0);
    REQUIRE(color.y() == 0);
    REQUIRE(color.z() == 0);
}

TEST_CASE("Framebuffer can set and get a pixel")
{
    Framebuffer framebuffer(800, 450);

    vec3 color(1, 0.5, 0.25);

    framebuffer.setPixel(100, 200, color);

    vec3 result = framebuffer.getPixel(100, 200);

    REQUIRE(result.x() == Catch::Approx(1.0));
    REQUIRE(result.y() == Catch::Approx(0.5));
    REQUIRE(result.z() == Catch::Approx(0.25));
}

TEST_CASE("Framebuffer ignores pixels outside the image")
{
    Framebuffer framebuffer(800, 450);

    framebuffer.setPixel(-1, -1, vec3(1, 1, 1));
    framebuffer.setPixel(800, 450, vec3(1, 1, 1));

    vec3 color = framebuffer.getPixel(0, 0);

    REQUIRE(color.x() == 0);
    REQUIRE(color.y() == 0);
    REQUIRE(color.z() == 0);
}