#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "vec3.h"

TEST_CASE("vec3 construction and component access")
{
    vec3 v(1.0, 2.0, 3.0);

    constexpr double eps = 1e-9;

    REQUIRE_THAT(v.x(), Catch::Matchers::WithinAbs(1.0, eps));
    REQUIRE_THAT(v.y(), Catch::Matchers::WithinAbs(2.0, eps));
    REQUIRE_THAT(v.z(), Catch::Matchers::WithinAbs(3.0, eps));
}

TEST_CASE("vec3 addition and subtraction")
{
    vec3 a(1.0, 2.0, 3.0);
    vec3 b(4.0, 5.0, 6.0);

    vec3 sum = a + b;
    vec3 difference = b - a;

    constexpr double eps = 1e-9;

    REQUIRE_THAT(sum.x(), Catch::Matchers::WithinAbs(5.0, eps));
    REQUIRE_THAT(sum.y(), Catch::Matchers::WithinAbs(7.0, eps));
    REQUIRE_THAT(sum.z(), Catch::Matchers::WithinAbs(9.0, eps));

    REQUIRE_THAT(difference.x(), Catch::Matchers::WithinAbs(3.0, eps));
    REQUIRE_THAT(difference.y(), Catch::Matchers::WithinAbs(3.0, eps));
    REQUIRE_THAT(difference.z(), Catch::Matchers::WithinAbs(3.0, eps));
}

TEST_CASE("vec3 scalar multiplication and division")
{
    vec3 v(2.0, 4.0, 6.0);

    vec3 multiplied = v * 3.0;
    vec3 divided = v / 2.0;

    constexpr double eps = 1e-9;

    REQUIRE_THAT(multiplied.x(), Catch::Matchers::WithinAbs(6.0, eps));
    REQUIRE_THAT(multiplied.y(), Catch::Matchers::WithinAbs(12.0, eps));
    REQUIRE_THAT(multiplied.z(), Catch::Matchers::WithinAbs(18.0, eps));

    REQUIRE_THAT(divided.x(), Catch::Matchers::WithinAbs(1.0, eps));
    REQUIRE_THAT(divided.y(), Catch::Matchers::WithinAbs(2.0, eps));
    REQUIRE_THAT(divided.z(), Catch::Matchers::WithinAbs(3.0, eps));
}

TEST_CASE("vec3 dot product")
{
    vec3 a(1.0, 2.0, 3.0);
    vec3 b(4.0, 5.0, 6.0);

    double result = dot(a, b);

    constexpr double eps = 1e-9;

    REQUIRE_THAT(result, Catch::Matchers::WithinAbs(32.0, eps));
}
TEST_CASE("vec3 cross product")
{
    vec3 a(1.0, 0.0, 0.0);
    vec3 b(0.0, 1.0, 0.0);

    vec3 result = cross(a, b);

    constexpr double eps = 1e-9;

    REQUIRE_THAT(result.x(), Catch::Matchers::WithinAbs(0.0, eps));
    REQUIRE_THAT(result.y(), Catch::Matchers::WithinAbs(0.0, eps));
    REQUIRE_THAT(result.z(), Catch::Matchers::WithinAbs(1.0, eps));
}

TEST_CASE("vec3 length and length squared")
{
    vec3 v(3.0, 4.0, 0.0);

    constexpr double eps = 1e-9;

    REQUIRE_THAT(v.length_squared(), Catch::Matchers::WithinAbs(25.0, eps));
    REQUIRE_THAT(v.length(), Catch::Matchers::WithinAbs(5.0, eps));
}

TEST_CASE("vec3 unit vector")
{
    vec3 v(3.0, 4.0, 0.0);

    vec3 result = unit_vector(v);

    constexpr double eps = 1e-9;

    REQUIRE_THAT(result.x(), Catch::Matchers::WithinAbs(0.6, eps));
    REQUIRE_THAT(result.y(), Catch::Matchers::WithinAbs(0.8, eps));
    REQUIRE_THAT(result.z(), Catch::Matchers::WithinAbs(0.0, eps));

    REQUIRE_THAT(result.length(), Catch::Matchers::WithinAbs(1.0, eps));
}