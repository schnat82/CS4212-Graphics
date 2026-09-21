#include <catch2/catch_test_macros.hpp>

#include "ray.h"


TEST_CASE("Ray construction stores origin and direction")
{
    point origin(1, 2, 3);
    vec3 direction(4, 5, 6);

    ray r(origin, direction);

    REQUIRE(r.origin().x() == origin.x());
    REQUIRE(r.origin().y() == origin.y());
    REQUIRE(r.origin().z() == origin.z());

    REQUIRE(r.direction().x() == direction.x());
    REQUIRE(r.direction().y() == direction.y());
    REQUIRE(r.direction().z() == direction.z());
}


TEST_CASE("Ray at t=0 returns the origin")
{
    ray r(point(1, 2, 3), vec3(4, 5, 6));

    point result = r.at(0);

    REQUIRE(result.x() == 1);
    REQUIRE(result.y() == 2);
    REQUIRE(result.z() == 3);
}


TEST_CASE("Ray at t=1 returns origin plus direction")
{
    ray r(point(1, 2, 3), vec3(4, 5, 6));

    point result = r.at(1);

    REQUIRE(result.x() == 5);
    REQUIRE(result.y() == 7);
    REQUIRE(result.z() == 9);
}


TEST_CASE("Ray handles positive t values")
{
    ray r(point(1, 2, 3), vec3(2, 4, 6));

    point result = r.at(2.5);

    REQUIRE(result.x() == 6);
    REQUIRE(result.y() == 12);
    REQUIRE(result.z() == 18);
}


TEST_CASE("Ray handles negative t values")
{
    ray r(point(10, 10, 10), vec3(2, 4, 6));

    point result = r.at(-2);

    REQUIRE(result.x() == 6);
    REQUIRE(result.y() == 2);
    REQUIRE(result.z() == -2);
}


TEST_CASE("Ray does not modify origin or direction")
{
    point original_origin(1, 2, 3);
    vec3 original_direction(4, 5, 6);

    ray r(original_origin, original_direction);

    r.at(5);
    r.at(-3);
    r.at(100);

    REQUIRE(r.origin().x() == 1);
    REQUIRE(r.origin().y() == 2);
    REQUIRE(r.origin().z() == 3);

    REQUIRE(r.direction().x() == 4);
    REQUIRE(r.direction().y() == 5);
    REQUIRE(r.direction().z() == 6);
}


TEST_CASE("Ray calculates fractional values within floating point tolerance")
{
    ray r(point(1, 2, 3), vec3(2, 4, 6));

    point result = r.at(0.5);

    REQUIRE(result.x() == 2.0);
    REQUIRE(result.y() == 4.0);
    REQUIRE(result.z() == 6.0);
}