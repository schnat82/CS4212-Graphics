CS4212 Computer Graphics — Ray Tracer and BVH Extension

Project Overview

This project is a C++ ray tracer developed for CS4212 Computer Graphics. It began with basic vector and ray calculations and has been expanded to support multiple geometric objects, lighting models, shadows, reflections, anti-aliasing, parallel rendering, and a Bounding Volume Hierarchy (BVH) acceleration structure.

The renderer generates PNG images using a virtual camera that casts rays into a 3D scene. The scene determines which objects the rays intersect and calculates pixel colors using the assigned shaders.

The most recent extension adds BVH acceleration and a performance comparison between BVH traversal and traditional brute-force intersection testing.

Implemented Features

Vector and Ray Mathematics

The renderer supports:

3D vectors and points

Vector addition, subtraction, and scaling

Dot products and cross products

Vector normalization

Ray origins and direction vectors

Calculating points along rays using P(t) = O + tD

These operations form the foundation for intersection testing, camera rays, and lighting calculations.

Camera and Framebuffer

A perspective camera generates rays through the image plane.

The framebuffer stores the calculated pixel colors and supports exporting rendered images as PNG files.

The renderer allows the image resolution to be configured through command-line arguments.

Geometric Objects and Intersection Testing

The renderer supports sphere and triangle objects.

Each object implements a ray-intersection function that determines whether a ray hits the object and records information about the intersection.

The hit record stores information including:

Intersection distance

Intersection position

Surface normal

Viewing direction

Associated shader

Shading and Lighting

Several shading techniques have been implemented:

Lambertian shading: Calculates diffuse lighting based on the angle between the surface normal and the direction toward a light source.

Blinn-Phong shading: Adds specular highlights to diffuse surfaces.

Normal visualization: Displays surface normals as colors to help debug geometry and intersection calculations.

Mirror shading: Generates reflected rays to simulate reflective surfaces.

Diffuse shading: Supports diffuse ray scattering.

The scene also supports multiple point lights with different positions and intensities.

Shadows

Shadow rays are cast from surface intersection points toward light sources.

If another object intersects the shadow ray before it reaches the light, that light is considered blocked.

The same intersection system is used for both primary rays and shadow rays.

Recursive Ray Tracing

The renderer supports recursive ray tracing for reflective and scattering materials.

A configurable maximum recursion depth prevents unlimited ray tracing.

Anti-Aliasing

The renderer supports multiple rays per pixel to reduce jagged edges.

Jittered sampling distributes rays across each pixel. When the number of rays per pixel is a perfect square, the renderer uses stratified jittered sampling.

For example, 100 rays per pixel can be distributed across a 10 × 10 sampling grid.

Parallel Rendering

OpenMP is used to distribute image rendering across multiple CPU threads.

The number of CPU threads can be configured using the command line.

Each image row can be rendered independently, allowing the ray tracer to take advantage of multicore processors.

Bounding Volume Hierarchy (BVH) Extension

Purpose

The BVH extension was implemented to improve ray-intersection performance in scenes containing many geometric objects.

Originally, the renderer used brute-force intersection testing. Every ray was tested against every object in the scene to determine the closest intersection.

This approach is straightforward but becomes expensive as the number of objects increases.

A BVH groups objects inside bounding boxes, allowing the renderer to skip entire groups when a ray does not intersect their bounding boxes.

Axis-Aligned Bounding Boxes

The new src/aabb.h file implements axis-aligned bounding boxes.

Each bounding box stores minimum and maximum coordinates along the X, Y, and Z axes.

The ray-box intersection test uses the slab method to determine whether a ray passes through the box.

The implementation also handles ray directions that are zero or nearly zero along an axis.

BVH Construction

The new src/bvh.h file implements the BVH node structure.

The hierarchy is constructed recursively:

Calculate the bounding box containing the current group of objects.

Determine which axis has the largest spatial extent.

Divide the objects around the median along that axis.

Recursively build child nodes.

Store the combined bounding box for the node.

Leaf nodes reference actual scene objects, while internal nodes reference smaller groups of objects.

This creates a binary tree that can be traversed during ray-intersection testing.

Bounding Boxes for Shapes

The Shape interface was extended to require a boundingBox() function.

Sphere: The bounding box is calculated using the sphere's center and radius.

Triangle: The bounding box is calculated from the minimum and maximum coordinates of its three vertices. A small amount of padding is added to avoid problems with extremely thin bounding boxes.

BVH Integration

The Scene class now supports both BVH and brute-force intersection testing.

The following functionality was added:

buildBVH() constructs the hierarchy from scene objects.

setUseBVH() enables or disables BVH traversal.

closestIntersection() selects the appropriate intersection method.

Adding a new object invalidates the existing hierarchy so it can be rebuilt.

BVH acceleration is used for primary rays, shadow rays, and recursive rays because they all rely on the scene's intersection functions.

The BVH is built before parallel rendering begins.

Optimized BVH Traversal

After the initial implementation, BVH traversal was improved.

The optimized version:

Tests whether a ray intersects the current node's bounding box.

Checks which child bounding boxes the ray could enter.

Visits the nearer child first.

Tracks the closest intersection found.

Skips the farther child when its bounding box cannot contain a closer intersection.

This reduces unnecessary object-intersection tests and recursive traversal.

The optimization is especially useful in scenes with many objects.

BVH Toggle

BVH is enabled by default in the updated test_scene executable.

To disable it, add:

--no-bvh

This allows both algorithms to be benchmarked using the same scene, image resolution, sampling settings, and thread count.

Test Scenes

Original Scene

The original test scene contains 13 geometric objects:

5 triangles

8 spheres

It includes a ground plane, mirror wall, floating triangle, and several spheres with different shaders.

The scene also contains two point lights and uses recursive reflections, shadows, and anti-aliasing.

This scene is useful for verifying that the renderer produces consistent results with BVH enabled and disabled.

Dense BVH Benchmark Scene

A second benchmark scene adds 400 small spheres in a 20 × 20 grid.

The grid is generated using nested loops with deterministic object positions.

Combined with the original scene, the dense benchmark contains 413 objects.

This scene is designed to demonstrate the benefits of BVH acceleration when the renderer must process a larger number of objects.

Both rendering modes use the same geometry and camera settings, allowing a more meaningful performance comparison.

Building the Renderer

The project uses CMake and vcpkg. General installation and dependency instructions are provided earlier in this README.

Windows Build

From the repository root, configure the development preset:

cmake --preset dev

Build the Release configuration:

cmake --build buildDev --config Release

The test scene executable should be located at:

buildDev/examples/Release/test_scene.exe

If a different CMake preset or build directory is used, adjust the executable path accordingly.

Running Tests

Where supported by the configured build, run:

ctest --test-dir buildDev -C Release --output-on-failure

Running the Ray Tracer

Render Without BVH

.\buildDev\examples\Release\test_scene.exe --width 1000 --height 1000 --rpp 16 --numcpus 4 --outputfile no_bvh.png --no-bvh

Render With BVH

.\buildDev\examples\Release\test_scene.exe --width 1000 --height 1000 --rpp 16 --numcpus 4 --outputfile with_bvh.png

Both commands use:

Resolution: 1000 × 1000 pixels

Rays per pixel: 16

CPU threads: 4

Identical scene geometry and lighting

The output filenames differ so the rendered images can be compared.

Quick Benchmark

For a faster initial comparison, use a smaller image and fewer rays per pixel:

Without BVH:

.\buildDev\examples\Release\test_scene.exe --width 500 --height 500 --rpp 4 --numcpus 4 --outputfile dense_no_bvh.png --no-bvh

With BVH:

.\buildDev\examples\Release\test_scene.exe --width 500 --height 500 --rpp 4 --numcpus 4 --outputfile dense_bvh.png

Performance Comparison

Initial Benchmark Results

The first benchmark was performed using the original scene containing 13 objects.

Rendering Mode

Time

Brute force (BVH disabled)

6.9 seconds

Initial BVH implementation

10.0 seconds

The initial BVH implementation was approximately 44.9% slower than brute-force rendering.

The measured speedup was:

Speedup = 6.9 / 10.0 = 0.69x

Why Was the Initial BVH Slower?

The original scene contained only 13 objects.

With so few objects, brute-force intersection testing was relatively inexpensive. BVH traversal introduced additional operations, including bounding-box intersection tests, recursive function calls, and pointer access.

The initial BVH implementation also performed unnecessary child traversal.

As a result, the overhead of using the BVH exceeded the intersection work it eliminated.

Improvements Made

The BVH traversal was optimized to prioritize nearer child nodes and skip child branches that cannot contain a closer intersection.

A dense scene with 400 additional spheres was also introduced to provide a more appropriate performance benchmark.

Dense Scene Benchmark Results

The following results should be recorded after running the optimized BVH against the dense scene:

Rendering Mode

Time

Brute force (413 objects)

Not yet measured

Optimized BVH (413 objects)

Not yet measured

Speedup

Not yet calculated

Calculate speedup using:

Speedup = Brute-force time / BVH time

A value greater than 1.0x indicates that BVH acceleration is faster.

The optimized BVH is expected to be more beneficial as scene complexity increases, but actual results must be measured rather than assumed.

Benchmark Verification

For a fair comparison:

Build both versions using the same Release configuration.

Use identical geometry, lighting, resolution, ray count, and thread count.

Run each configuration multiple times to reduce timing variability.

Compare the output images to confirm that BVH traversal does not change the intended rendering.

Record the measured execution times and calculated speedup.

The renderer prints total wall-clock execution time to the terminal. This includes more than intersection traversal alone, so the measured speedup represents the overall program rather than just the BVH traversal function.

New and Modified Files

File

Description

src/aabb.h

New axis-aligned bounding box implementation

src/bvh.h

New BVH construction and optimized traversal

src/shape.h

Added bounding-box interface

src/sphere.h

Added sphere bounding-box calculation

src/triangle.h

Added triangle bounding-box calculation

src/scene.h

Added BVH configuration and storage

src/scene.cpp

Integrated BVH and brute-force intersection paths

examples/test_scene.cpp

Added BVH toggle, execution timing, and dense benchmark scene

README.md

Updated implementation, build, testing, and performance documentation

The BVH and AABB implementations are header-only, so no additional .cpp files are required for them.

Lessons Learned

This extension demonstrates that acceleration structures are not automatically faster in every situation.

For small scenes, the overhead of constructing and traversing a BVH can outweigh the cost of directly checking each object.

For larger scenes, BVH traversal can reduce the number of expensive geometric intersection tests by eliminating groups of objects that a ray cannot hit.

The project also demonstrates the importance of benchmarking algorithms under identical conditions, optimizing traversal order, and verifying that performance improvements do not change the rendered output.

Future Improvements

Possible future improvements include:

Flat, array-based BVH storage to improve memory locality

Surface-area heuristic (SAH) splitting instead of median splitting

BVH visualization for debugging

More complex scenes containing thousands of objects

Separate timing measurements for BVH construction and ray traversal

Integration of BVH acceleration into additional scene-rendering executables

Automated image comparisons between BVH and brute-force output

These improvements would help make the renderer more efficient and scalable as scene complexity increases.