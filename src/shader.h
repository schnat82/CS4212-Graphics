#ifndef SHADER_H
#define SHADER_H

#include "vec3.h"

struct HitRecord;

class Shader {
public:
    virtual ~Shader() = default;
    virtual vec3 rayColor(const HitRecord& hit) const = 0;
};

#endif
