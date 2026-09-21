FOR LAB 2

# CS4212 Computer Graphics

This project contains basic classes and tools used to begin building a ray tracer for CS4212 Computer Graphics.

## Current Features

The project currently includes:

* `vec3` class for 3D vectors and points
* `ray` class containing an origin and direction
* `ray::at(t)` for finding points along a ray
* Abstract `Camera` base class
* `Framebuffer` class for storing image pixels
* Catch2 unit tests for vec3, ray, and framebuffer
* PNG image generation examples

## Ray Class

A ray is represented using an origin and direction.

The `at(t)` function calculates a point along the ray using:

`P(t) = origin + t * direction`

The ray tests check:

* Ray construction
* `at(0)`
* `at(1)`
* Positive t values
* Negative t values
* Origin and direction immutability
* Fractional values

## Compiling

Open a PowerShell terminal and navigate to the project:

```powershell
cd C:\Users\schmu\Documents\CS4212\starterCode
```

Configure the project with CMake and vcpkg:

```powershell
cmake -S . -B buildDev -DCMAKE_TOOLCHAIN_FILE="C:/Users/schmu/Documents/CS4212/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

Compile the project:

```powershell
cmake --build buildDev
```

## Running Tests

Run all tests:

```powershell
ctest --test-dir buildDev --output-on-failure
```

The `Simple Failure` test is intentionally designed to fail.

Run only the ray tests:

```powershell
.\buildDev\utests\Debug\utest_ray.exe
```

Run the vec3 tests:

```powershell
.\buildDev\utests\Debug\utest_vec3.exe
```

Run the framebuffer tests:

```powershell
.\buildDev\utests\Debug\utest_framebuffer.exe
```

## Generating an Image

Run the PNG example:

```powershell
.\buildDev\examples\Debug\test_pngWrite.exe
```

This generates example PNG images, including a checkerboard.

The checkerboard can be opened with:

```powershell
start .\checkerboard.png
```

The project now has the basic vector, ray, camera, framebuffer, testing, and image generation functionality needed to continue developing a ray tracer.
