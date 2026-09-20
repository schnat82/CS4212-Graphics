#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "vec3.h"
#include <vector>

class Framebuffer {
public:
    Framebuffer(int width, int height);

    int width() const;
    int height() const;

    void setPixel(int x, int y, const vec3& color);
    vec3 getPixel(int x, int y) const;

private:
    int imageWidth;
    int imageHeight;
    std::vector<vec3> pixels;
};

#endif