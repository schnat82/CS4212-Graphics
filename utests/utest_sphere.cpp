#include <catch2/catch_test_macros.hpp>

#include "sphere.h"

TEST_CASE("Sphere miss returns false")
{
    Sphere sphere(point(0, 0, -5), 1.0);
    ray r(point(0, 0, 0), vec3(0, 1, 0));

    REQUIRE_FALSE(sphere.intersect(r));
}

TEST_CASE("Sphere hit through center returns true")
{
    Sphere sphere(point(0, 0, -5), 1.0);
    ray r(point(0, 0, 0), vec3(0, 0, -1));

    REQUIRE(sphere.intersect(r));
}

TEST_CASE("Sphere tangent ray counts as an intersection")
{
    Sphere sphere(point(0, 0, -5), 1.0);
    ray r(point(1, 0, 0), vec3(0, 0, -1));

    REQUIRE(sphere.intersect(r));
}

TEST_CASE("Ray starting inside sphere intersects")
{
    Sphere sphere(point(0, 0, 0), 2.0);
    ray r(point(0, 0, 0), vec3(1, 0, 0));

    REQUIRE(sphere.intersect(r));
}

TEST_CASE("Sphere stores center and radius")
{
    Sphere sphere(point(1, 2, 3), 4.5);

    REQUIRE(sphere.center().x() == 1);
    REQUIRE(sphere.center().y() == 2);
    REQUIRE(sphere.center().z() == 3);
    REQUIRE(sphere.radius() == 4.5);
}
