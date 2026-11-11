#version 330 core

layout(location = 0) out vec4 fragmentColor;

uniform vec3 diffuseComponent;
uniform vec3 specularComponent;
uniform float phongExponent;
uniform vec3 eyePosWorld;
uniform vec4 lightPosWorld;

in vec4 normal;
in vec4 lightDir;
in vec3 worldPosition;

void main(void)
{
    vec3 N = normalize(normal.xyz);
    vec3 L = normalize(lightPosWorld.xyz - worldPosition);
    vec3 V = normalize(eyePosWorld - worldPosition);
    vec3 H = normalize(L + V);

    float ndotl = max(dot(N, L), 0.0);
    float specular = ndotl > 0.0 ? pow(max(dot(N, H), 0.0), phongExponent) : 0.0;

    vec3 diffuseShading = diffuseComponent * ndotl;
    vec3 specularShading = specularComponent * specular;
    fragmentColor = vec4(diffuseShading + specularShading, 1.0);
}
