#ifndef CS4212_BVH_H
#define CS4212_BVH_H

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include "shape.h"

class BVHNode : public Shape {
public:
    explicit BVHNode(
        std::vector<std::shared_ptr<Shape>> objects
    ) : BVHNode(objects, 0, objects.size()) {}

    bool intersect(
        const ray& r,
        double tMin,
        double tMax,
        HitRecord& hit
    ) const override {
        if (!box.hit(r, tMin, tMax)) {
            return false;
        }

        // Leaf: intersect the actual shape.
        if (!right) {
            if (!left->intersect(r, tMin, tMax, hit)) {
                return false;
            }

            hit.shader = left->getShader();
            return true;
        }

        double leftEntry = tMin;
        double rightEntry = tMin;

        const bool leftPossible = boxEntry(
            left->boundingBox(), r, tMin, tMax, leftEntry
        );

        const bool rightPossible = boxEntry(
            right->boundingBox(), r, tMin, tMax, rightEntry
        );

        if (!leftPossible && !rightPossible) {
            return false;
        }

        const Shape* first = left.get();
        const Shape* second = right.get();

        double firstEntry = leftEntry;
        double secondEntry = rightEntry;

        bool firstPossible = leftPossible;
        bool secondPossible = rightPossible;

        if ((!leftPossible && rightPossible) ||
            (leftPossible && rightPossible &&
             rightEntry < leftEntry)) {
            std::swap(first, second);
            std::swap(firstEntry, secondEntry);
            std::swap(firstPossible, secondPossible);
        }

        bool found = false;
        double closestT = tMax;
        HitRecord candidate{};

        if (firstPossible &&
            first->intersect(r, tMin, closestT, candidate)) {
            found = true;
            closestT = candidate.t;
            hit = candidate;

            if (first->getShader()) {
                hit.shader = first->getShader();
            }
        }

        // Skip the second subtree when its nearest possible
        // intersection is already beyond the closest hit.
        if (secondPossible && secondEntry <= closestT) {
            if (second->intersect(
                r, tMin, closestT, candidate
            )) {
                found = true;
                hit = candidate;

                if (second->getShader()) {
                    hit.shader = second->getShader();
                }
            }
        }

        return found;
    }

    AABB boundingBox() const override {
        return box;
    }

private:
    std::shared_ptr<Shape> left;
    std::shared_ptr<Shape> right;
    AABB box;

    static bool boxEntry(
        const AABB& bounds,
        const ray& r,
        double tMin,
        double tMax,
        double& entry
    ) {
        for (int axis = 0; axis < 3; ++axis) {
            const double d = r.direction()[axis];
            const double o = r.origin()[axis];

            if (std::abs(d) < 1e-15) {
                if (o < bounds.minimum[axis] ||
                    o > bounds.maximum[axis]) {
                    return false;
                }
                continue;
            }

            double a = (bounds.minimum[axis] - o) / d;
            double b = (bounds.maximum[axis] - o) / d;

            if (a > b) {
                std::swap(a, b);
            }

            tMin = std::max(tMin, a);
            tMax = std::min(tMax, b);

            if (tMax < tMin) {
                return false;
            }
        }

        entry = tMin;
        return true;
    }

    BVHNode(
        std::vector<std::shared_ptr<Shape>>& objects,
        size_t start,
        size_t end
    ) {
        const size_t count = end - start;

        AABB bounds = objects[start]->boundingBox();

        for (size_t i = start + 1; i < end; ++i) {
            bounds = surroundingBox(
                bounds,
                objects[i]->boundingBox()
            );
        }

        box = bounds;

        const vec3 extent =
            bounds.maximum - bounds.minimum;

        int axis = 0;

        if (extent[1] > extent[axis]) {
            axis = 1;
        }

        if (extent[2] > extent[axis]) {
            axis = 2;
        }

        if (count == 1) {
            left = objects[start];
            return;
        }

        if (count == 2) {
            left = objects[start];
            right = objects[start + 1];
            return;
        }

        const size_t mid = start + count / 2;

        std::nth_element(
            objects.begin() + start,
            objects.begin() + mid,
            objects.begin() + end,
            [axis](const auto& a, const auto& b) {
                const AABB aa = a->boundingBox();
                const AABB bb = b->boundingBox();

                return aa.minimum[axis] + aa.maximum[axis] <
                       bb.minimum[axis] + bb.maximum[axis];
            }
        );

        left = std::shared_ptr<Shape>(
            new BVHNode(objects, start, mid)
        );

        right = std::shared_ptr<Shape>(
            new BVHNode(objects, mid, end)
        );
    }
};

#endif
