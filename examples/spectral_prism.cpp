
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "camera.h"
#include "framebuffer.h"
#include "handleGraphicsArgs.h"
#include "png++/png.hpp"
#include "ray.h"
#include "vec3.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace {

constexpr double EPS = 1e-5;

using V = vec3;
using P = point;

V clampColor(const V& c) {
    return V(
        std::max(0.0, c.x()),
        std::max(0.0, c.y()),
        std::max(0.0, c.z())
    );
}

V mix(const V& a, const V& b, double t) {
    return (1.0 - t) * a + t * b;
}

V safeUnit(const V& v) {
    return v.length_squared() < 1e-20
        ? V(0, 1, 0)
        : unit_vector(v);
}

V reflect(const V& d, const V& n) {
    return d - 2.0 * dot(d, n) * n;
}

double smooth(double a, double b, double x) {
    double t = std::clamp((x - a) / (b - a), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

double gauss(double x, double sigma) {
    return std::exp(-0.5 * x * x / (sigma * sigma));
}

// ------------------------------------------------------
// Geometry and light definitions
// ------------------------------------------------------

struct PrismTriangle {
    P a, b, c;
    V normal;
};

struct Hit {
    double t = 1e30;
    P p;
    V n;
    int material = -1;
};

struct Beam {
    P a, b;
    V color;
    double radius;
    double strength;
};

std::vector<PrismTriangle> prism;
std::vector<Beam> luminous;
std::vector<Beam> casts;

void addTriangle(P a, P b, P c) {
    prism.push_back({
        a, b, c,
        safeUnit(cross(b - a, c - a))
    });
}

void addQuad(P a, P b, P c, P d) {
    addTriangle(a, b, c);
    addTriangle(a, c, d);
}

// ------------------------------------------------------
// Ray-triangle intersection
// Moller-Trumbore algorithm
// ------------------------------------------------------

bool triangleHit(
    const ray& r,
    const PrismTriangle& tr,
    double& t,
    V& normal
) {
    V e1 = tr.b - tr.a;
    V e2 = tr.c - tr.a;
    V p = cross(r.direction(), e2);

    double det = dot(e1, p);

    if (std::abs(det) < 1e-9)
        return false;

    double inv = 1.0 / det;

    V s = r.origin() - tr.a;
    double u = dot(s, p) * inv;

    if (u < 0.0 || u > 1.0)
        return false;

    V q = cross(s, e1);
    double v = dot(r.direction(), q) * inv;

    if (v < 0.0 || u + v > 1.0)
        return false;

    t = dot(e2, q) * inv;
    normal = tr.normal;

    return t > EPS;
}

bool hitPrism(
    const ray& r,
    double lo,
    double hi,
    Hit& out
) {
    bool found = false;

    for (const auto& tr : prism) {
        double t;
        V n;

        if (triangleHit(r, tr, t, n)
            && t >= lo
            && t < hi) {

            hi = t;
            out.t = t;
            out.p = r.at(t);
            out.n = n;
            out.material = 1;
            found = true;
        }
    }

    return found;
}

// ------------------------------------------------------
// Scene intersection
// ------------------------------------------------------

bool hitScene(const ray& r, Hit& out) {
    double closest = 1e30;

    bool found = hitPrism(
        r, EPS, closest, out
    );

    if (found)
        closest = out.t;

    // Dark graphite floor below the prism.
    if (std::abs(r.direction().y()) > 1e-9) {
        double t =
            (-1.35 - r.origin().y())
            / r.direction().y();

        if (t > EPS && t < closest) {
            out.t = t;
            out.p = r.at(t);
            out.n = V(0, 1, 0);
            out.material = 0;
            found = true;
        }
    }

    return found;
}

// ------------------------------------------------------
// Reflection and refraction
// Snell's law and Schlick's approximation
// ------------------------------------------------------

bool refractDirection(
    V incident,
    V outward,
    double glassIOR,
    V& refracted,
    double& fresnel
) {
    bool entering = dot(incident, outward) < 0.0;

    V n = entering ? outward : -outward;

    double etai = entering ? 1.0 : glassIOR;
    double etat = entering ? glassIOR : 1.0;

    double cosI = std::clamp(
        -dot(incident, n), 0.0, 1.0
    );

    double ratio = etai / etat;

    double sin2 =
        ratio * ratio * (1.0 - cosI * cosI);

    double r0 = std::pow(
        (etai - etat) / (etai + etat), 2.0
    );

    fresnel = r0
        + (1.0 - r0) * std::pow(1.0 - cosI, 5.0);

    // Total internal reflection.
    if (sin2 >= 1.0) {
        fresnel = 1.0;
        return false;
    }

    refracted = safeUnit(
        ratio * incident
        + (ratio * cosI - std::sqrt(1.0 - sin2)) * n
    );

    return true;
}

// ------------------------------------------------------
// Background and Blinn-Phong floor
// ------------------------------------------------------

V background(const V& d) {
    double v = std::clamp(
        d.y() * 0.5 + 0.5,
        0.0, 1.0
    );

    return mix(
        V(0.012, 0.020, 0.040),
        V(0.002, 0.004, 0.013),
        v
    );
}

V shadeSurface(
    const P& p,
    V normal,
    const V& view
) {
    V base(0.07, 0.09, 0.13);

    double checker = std::fmod(
        std::floor(p.x() * 1.3)
        + std::floor(p.z() * 1.3)
        + 2000.0,
        2.0
    );

    base *= checker < 0.5 ? 1.13 : 0.83;

    V lightDir = safeUnit(
        V(-4, 7, 3) - p
    );

    double diffuse = std::max(
        0.0, dot(normal, lightDir)
    );

    V halfVector = safeUnit(
        lightDir + view
    );

    double specular = std::pow(
        std::max(0.0, dot(normal, halfVector)),
        100.0
    );

    return base * (0.14 + 0.8 * diffuse)
        + V(0.33, 0.42, 0.55) * specular;
}

// ------------------------------------------------------
// Recursive ray tracing for reflective glass
// ------------------------------------------------------

V trace(
    ray r,
    int depth,
    double ior
) {
    if (depth <= 0)
        return V(0, 0, 0);

    Hit h;

    if (!hitScene(r, h)) {
        return background(
            safeUnit(r.direction())
        );
    }

    V d = safeUnit(r.direction());

    if (h.material == 0) {
        return shadeSurface(
            h.p,
            V(0, 1, 0),
            -d
        );
    }

    V refracted;
    double fresnel;

    bool transmits = refractDirection(
        d, h.n, ior,
        refracted, fresnel
    );

    // Reflected ray.
    V reflectionDirection = safeUnit(
        reflect(d, h.n)
    );

    V reflectionColor = trace(
        ray(
            h.p + EPS * reflectionDirection,
            reflectionDirection
        ),
        depth - 1,
        ior
    );

    // Transmitted ray.
    V transmissionColor(0, 0, 0);

    if (transmits) {
        transmissionColor = trace(
            ray(
                h.p + EPS * refracted,
                refracted
            ),
            depth - 1,
            ior
        );
    }

    // Bright edges suggest polished glass.
    double edge = std::pow(
        1.0 - std::abs(
            dot(d, safeUnit(h.n))
        ),
        3.0
    );

    V rim = V(0.18, 0.50, 0.85)
        * (0.14 + 0.8 * edge);

    return reflectionColor * (fresnel * 0.9)
        + transmissionColor
          * ((1.0 - fresnel) * 0.84)
        + rim;
}

// ------------------------------------------------------
// Visible volumetric-style light beams
// ------------------------------------------------------

double raySegmentDistance(
    const P& ro,
    V rd,
    const P& a,
    const P& b,
    double& rayT,
    double& segU
) {
    V w = b - a;
    V diff = ro - a;

    double A = dot(rd, rd);
    double B = dot(rd, w);
    double C = dot(w, w);

    if (C < 1e-12) {
        segU = 0.0;
        rayT = std::max(
            0.0, -dot(rd, diff) / A
        );

        return (
            ro + rd * rayT - a
        ).length();
    }

    double D = dot(rd, diff);
    double E = dot(w, diff);
    double den = A * C - B * B;

    double s = den > 1e-12
        ? (B * E - C * D) / den
        : 0.0;

    double u = std::clamp(
        (B * s + E) / C, 0.0, 1.0
    );

    s = std::max(
        0.0, (B * u - D) / A
    );

    u = std::clamp(
        (B * s + E) / C, 0.0, 1.0
    );

    s = std::max(
        0.0, (B * u - D) / A
    );

    rayT = s;
    segU = u;

    return (
        ro + rd * s - (a + w * u)
    ).length();
}

V beamLight(
    const ray& r,
    double maxRayT
) {
    V rd = safeUnit(r.direction());
    V light(0, 0, 0);

    for (const auto& beam : luminous) {
        double t, u;

        double distance = raySegmentDistance(
            r.origin(),
            rd,
            beam.a,
            beam.b,
            t, u
        );

        if (t >= maxRayT)
            continue;

        double taper =
            smooth(0.0, 0.075, u)
            * smooth(0.0, 0.11, 1.0 - u);

        double core =
            gauss(distance, beam.radius * 0.29)
            * 1.3;

        double halo =
            gauss(distance, beam.radius)
            * 0.38;

        double haze =
            gauss(distance, beam.radius * 3.3)
            * 0.06;

        light += beam.color
            * beam.strength
            * (core + halo + haze)
            * (0.62 + 0.38 * taper);
    }

    return light;
}

// ------------------------------------------------------
// Rainbow illumination on the floor
// ------------------------------------------------------

V floorCaustics(const P& p) {
    V c(0, 0, 0);

    for (const auto& beam : casts) {
        V a = beam.a;
        V w = beam.b - beam.a;

        double den =
            w.x() * w.x()
            + w.z() * w.z();

        if (den < 1e-9)
            continue;

        double u = std::clamp(
            (
                (p.x() - a.x()) * w.x()
                + (p.z() - a.z()) * w.z()
            ) / den,
            0.0, 1.0
        );

        double xx =
            p.x() - (a.x() + w.x() * u);

        double zz =
            p.z() - (a.z() + w.z() * u);

        double distance =
            std::sqrt(xx * xx + zz * zz);

        c += beam.color * beam.strength
            * (
                gauss(distance, beam.radius * 0.42)
                * 0.24
                +
                gauss(distance, beam.radius * 2.5)
                * 0.04
            );
    }

    return c;
}

// ------------------------------------------------------
// Tone mapping and gamma correction
// ------------------------------------------------------

V toneMap(V c) {
    c = clampColor(c);

    return V(
        std::pow(
            c.x() / (1.0 + c.x()), 1.0 / 2.2
        ),
        std::pow(
            c.y() / (1.0 + c.y()), 1.0 / 2.2
        ),
        std::pow(
            c.z() / (1.0 + c.z()), 1.0 / 2.2
        )
    );
}

void addBeam(
    P a,
    P b,
    V color,
    double radius,
    double strength
) {
    luminous.push_back({
        a, b, color, radius, strength
    });
}

// ------------------------------------------------------
// Build a true 3D triangular glass prism
// ------------------------------------------------------

void makePrism() {
    const double z0 = -0.65;
    const double z1 = 0.65;

    P A(-1.13, -0.65, z0);
    P B( 1.13, -0.65, z0);
    P C( 0.00,  1.25, z0);

    P D(-1.13, -0.65, z1);
    P E( 1.13, -0.65, z1);
    P F( 0.00,  1.25, z1);

    // Front and back triangular faces.
    addTriangle(A, C, B);
    addTriangle(D, E, F);

    // Three rectangular sides,
    // each composed of two triangles.
    addQuad(A, B, E, D);
    addQuad(B, C, F, E);
    addQuad(C, A, D, F);
}

// ------------------------------------------------------
// Wavelength-dependent optical dispersion
// ------------------------------------------------------

void makeOptics() {
    const std::array<V, 9> colors = {
        V(1.00, 0.04, 0.10),
        V(1.00, 0.19, 0.02),
        V(1.00, 0.51, 0.02),
        V(1.00, 0.92, 0.03),
        V(0.25, 1.00, 0.06),
        V(0.03, 0.95, 0.70),
        V(0.04, 0.57, 1.00),
        V(0.22, 0.20, 1.00),
        V(0.67, 0.10, 1.00)
    };

    // Representative wavelengths in nanometers.
    const std::array<double, 9> wavelengths = {
        670, 620, 590,
        565, 530, 500,
        475, 445, 415
    };

    P entry(-0.55, 0.32522, -0.02);
    P source(-5.35, 0.85, 1.15);

    V incoming = safeUnit(entry - source);

    // Single incoming white light.
    addBeam(
        source, entry,
        V(1, 1, 1),
        0.042, 1.35
    );

    // Outward normal of the prism's left face.
    V inNormal = prism.back().normal;

    // A faint surface reflection.
    V ghost = safeUnit(
        reflect(incoming, inNormal)
    );

    addBeam(
        entry,
        entry + ghost * 1.5,
        V(0.28, 0.39, 0.55),
        0.015, 0.20
    );

    // Calculate a separate optical path
    // for each sampled wavelength.
    for (size_t i = 0;
         i < wavelengths.size();
         ++i) {

        double lambda = wavelengths[i] / 1000.0;

        // Cauchy's equation: shorter wavelengths
        // have a higher refractive index.
        double ior =
            1.48 + 0.0150 / (lambda * lambda);

        V internalDirection;
        double fresnel = 0.0;

        if (!refractDirection(
            incoming,
            inNormal,
            ior,
            internalDirection,
            fresnel
        )) {
            continue;
        }

        // Trace the beam through the prism.
        P inside = entry
            + internalDirection * 0.002;

        Hit exitHit;

        if (!hitPrism(
            ray(inside, internalDirection),
            EPS,
            100.0,
            exitHit
        )) {
            continue;
        }

        // Subtle colored glow inside the glass.
        addBeam(
            entry,
            exitHit.p,
            colors[i],
            0.020, 0.25
        );

        // Refraction as the light exits the glass.
        V outgoing;
        double exitFresnel = 0.0;

        if (!refractDirection(
            internalDirection,
            exitHit.n,
            ior,
            outgoing,
            exitFresnel
        )) {
            outgoing = safeUnit(
                reflect(
                    internalDirection,
                    exitHit.n
                )
            );
        }

        P end = exitHit.p
            + outgoing * 6.5;

        // Nine sampled bands create a continuous-
        // looking rainbow with antialiasing.
        addBeam(
            exitHit.p, end,
            colors[i],
            0.030, 1.4
        );

        // Stylized colored floor illumination.
        casts.push_back({
            exitHit.p,
            end,
            colors[i],
            0.12,
            0.55
        });
    }
}

} // namespace

// ------------------------------------------------------
// Main rendering program
// ------------------------------------------------------

int main(int argc, char** argv) {
    try {
        sivelab::GraphicsArgs args;
        args.process(argc, argv);

        if (args.width <= 0 ||
            args.height <= 0 ||
            args.rpp <= 0 ||
            args.recursionDepth <= 0 ||
            args.numCpus <= 0) {

            throw std::runtime_error(
                "width, height, rpp, recursionDepth "
                "and numcpus must be positive"
            );
        }

        if (args.withPreview ||
            args.useDepthOfField ||
            args.withIntersectTest ||
            args.withGridDim ||
            !args.inputFileName.empty()) {

            throw std::runtime_error(
                "Preview, depth-of-field, input and "
                "grid/intersection-test flags "
                "are not supported"
            );
        }

        makePrism();
        makeOptics();

        const int width = args.width;
        const int height = args.height;

        Framebuffer fb(width, height);

        double aspect =
            static_cast<double>(width) / height;

        // Elevated camera looking at the prism.
        P cam(0.0, 1.05, 8.9);
        V target(0.0, -0.18, 0.0);

        PerspectiveCamera camera(
            cam,
            target - cam,
            1.10,
            1.10 / aspect,
            0.94,
            width,
            height
        );

        // Stratified jittered anti-aliasing.
        int grid = static_cast<int>(
            std::sqrt(
                static_cast<double>(args.rpp)
            )
        );

        bool square = grid * grid == args.rpp;

#ifdef _OPENMP
        omp_set_dynamic(0);
        omp_set_num_threads(args.numCpus);

#pragma omp parallel for schedule(dynamic, 1)
#endif
        for (int y = 0; y < height; ++y) {

            // Every image row has its own RNG.
            std::mt19937 rng(
                0x51A7u
                + static_cast<unsigned>(y) * 14889u
            );

            std::uniform_real_distribution<double>
                random(0.0, 1.0);

            for (int x = 0; x < width; ++x) {
                V accumulated(0, 0, 0);

                for (int k = 0; k < args.rpp; ++k) {

                    double dx = square
                        ? (k % grid + random(rng)) / grid
                        : random(rng);

                    double dy = square
                        ? (k / grid + random(rng)) / grid
                        : random(rng);

                    ray cameraRay = camera.generateRay(
                        x + dx,
                        y + dy
                    );

                    V direction = safeUnit(
                        cameraRay.direction()
                    );

                    ray r(
                        cameraRay.origin(),
                        direction
                    );

                    Hit h;
                    bool found = hitScene(r, h);

                    // Recursive reflection/refraction.
                    V color = trace(
                        r,
                        args.recursionDepth,
                        1.51
                    );

                    // Colored light falling on the floor.
                    if (found && h.material == 0) {
                        color += floorCaustics(h.p);
                    }

                    // Light shafts and glowing beams.
                    double stop = found ? h.t : 1e30;
                    color += beamLight(r, stop);

                    // Very subtle stylized floating dust.
                    double dust =
                        std::sin((x + dx) * 0.157)
                        * std::sin((y + dy) * 0.217);

                    if (dust > 0.995) {
                        color += V(
                            0.02, 0.04, 0.055
                        ) * (
                            (dust - 0.995) / 0.005
                        );
                    }

                    accumulated += color;
                }

                fb.setPixel(
                    x,
                    y,
                    toneMap(
                        accumulated
                        / static_cast<double>(args.rpp)
                    )
                );
            }
        }

        // Write the resulting PNG image.
        png::image<png::rgb_pixel> image(
            width, height
        );

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                V c = fb.getPixel(x, y);

                auto channel = [](double v) {
                    return static_cast<png::byte>(
                        255 * std::clamp(
                            v, 0.0, 1.0
                        )
                    );
                };

                image[y][x] = png::rgb_pixel(
                    channel(c.x()),
                    channel(c.y()),
                    channel(c.z())
                );
            }
        }

        std::string name =
            args.outputFileName.empty()
                ? "spectral_prism.png"
                : args.outputFileName;

        image.write(name);

        std::cout
            << "Wrote " << name
            << " (" << width << "x" << height
            << ", " << args.rpp
            << " rays/pixel)\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr
            << "Render error: "
            << e.what() << '\n';

        return 1;
    }
}
