#include "framebuffer.h"

Framebuffer::Framebuffer(int width, int height)
    : imageWidth(width),
      imageHeight(height),
      pixels(width * height, vec3(0, 0, 0)) {
}

int Framebuffer::width() const {
    return imageWidth;
}

int Framebuffer::height() const {
    return imageHeight;
}

void Framebuffer::setPixel(int x, int y, const vec3& color) {
    if (x >= 0 && x < imageWidth && y >= 0 && y < imageHeight) {
        pixels[y * imageWidth + x] = color;
    }
}

vec3 Framebuffer::getPixel(int x, int y) const {
    if (x >= 0 && x < imageWidth && y >= 0 && y < imageHeight) {
        return pixels[y * imageWidth + x];
    }

    return vec3(0, 0, 0);
}