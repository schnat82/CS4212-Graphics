#include <catch2/catch_test_macros.hpp>

#include <limits>

#include "sphere.h"

TEST_CASE("Sphere miss returns false")
{
    Sphere sphere(point(0, 0, -5), 1.0);
    ray r(point(0, 0, 0), vec3(0, 1, 0));

    HitRecord hit;

    REQUIRE_FALSE(
        sphere.intersect(
            r,
            0.001,
            std::numeric_limits<double>::infinity(),
            hit
        )
    );
}

TEST_CASE("Sphere hit through center returns true")
{
    Sphere sphere(point(0, 0, -5), 1.0);
    ray r(point(0, 0, 0), vec3(0, 0, -1));

    HitRecord hit;

    REQUIRE(
        sphere.intersect(
            r,
            0.001,
            std::numeric_limits<double>::infinity(),
            hit
        )
    );

    // The front of the sphere is at z = -4.
    REQUIRE(hit.t == 4.0);
    REQUIRE(hit.p.x() == 0.0);
    REQUIRE(hit.p.y() == 0.0);
    REQUIRE(hit.p.z() == -4.0);
}

TEST_CASE("Sphere tangent ray counts as an intersection")
{
    Sphere sphere(point(0, 0, -5), 1.0);
    ray r(point(1, 0, 0), vec3(0, 0, -1));

    HitRecord hit;

    REQUIRE(
        sphere.intersect(
            r,
            0.001,
            std::numeric_limits<double>::infinity(),
            hit
        )
    );

    REQUIRE(hit.t == 5.0);
}

TEST_CASE("Ray starting inside sphere intersects")
{
    Sphere sphere(point(0, 0, 0), 2.0);
    ray r(point(0, 0, 0), vec3(1, 0, 0));

    HitRecord hit;

    REQUIRE(
        sphere.intersect(
            r,
            0.001,
            std::numeric_limits<double>::infinity(),
            hit
        )
    );

    // Starting at the center, the ray exits at x = 2.
    REQUIRE(hit.t == 2.0);
    REQUIRE(hit.p.x() == 2.0);
    REQUIRE(hit.p.y() == 0.0);
    REQUIRE(hit.p.z() == 0.0);
}

TEST_CASE("Sphere intersection respects t range")
{
    Sphere sphere(point(0, 0, -5), 1.0);
    ray r(point(0, 0, 0), vec3(0, 0, -1));

    HitRecord hit;

    // Sphere intersections occur at t = 4 and t = 6.
    // Neither should be accepted if tMax is 3.
    REQUIRE_FALSE(
        sphere.intersect(
            r,
            0.001,
            3.0,
            hit
        )
    );

    // If tMin is past the first intersection,
    // the second intersection at t = 6 should be found.
    REQUIRE(
        sphere.intersect(
            r,
            5.0,
            10.0,
            hit
        )
    );

    REQUIRE(hit.t == 6.0);
}

TEST_CASE("Sphere intersection calculates correct normal")
{
    Sphere sphere(point(0, 0, -5), 1.0);
    ray r(point(0, 0, 0), vec3(0, 0, -1));

    HitRecord hit;

    REQUIRE(
        sphere.intersect(
            r,
            0.001,
            std::numeric_limits<double>::infinity(),
            hit
        )
    );

    REQUIRE(hit.normal.x() == 0.0);
    REQUIRE(hit.normal.y() == 0.0);
    REQUIRE(hit.normal.z() == 1.0);
}

TEST_CASE("Sphere stores center and radius")
{
    Sphere sphere(point(1, 2, 3), 4.5);

    REQUIRE(sphere.center().x() == 1);
    REQUIRE(sphere.center().y() == 2);
    REQUIRE(sphere.center().z() == 3);
    REQUIRE(sphere.radius() == 4.5);
}