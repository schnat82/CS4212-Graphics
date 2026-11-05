#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 vertexColor;

void main(void)
{
    vertexColor = in_Color;
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
}
