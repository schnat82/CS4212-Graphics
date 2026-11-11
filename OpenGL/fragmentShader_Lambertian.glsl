#version 330 core

layout(location = 0) out vec4 fragmentColor;

uniform vec3 diffuseComponent;
uniform vec4 lightPosWorld;

in vec4 normal;
in vec4 lightDir;
in vec3 worldPosition;

void main(void)
{
    vec3 N = normalize(normal.xyz);
    // Recompute L per fragment from the interpolated world position.
    vec3 L = normalize(lightPosWorld.xyz - worldPosition);
    float val = max(0.0, dot(N, L));
    vec3 diffuseShading = diffuseComponent * val;
    fragmentColor = vec4(diffuseShading, 1.0);
}
