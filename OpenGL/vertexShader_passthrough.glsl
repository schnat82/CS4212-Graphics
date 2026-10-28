
#version 330 core

// Attribute location 0: Vertex position.
layout(location = 0) in vec3 in_Position;

// Attribute location 1: Vertex color.
layout(location = 1) in vec3 in_Color;

// Output color to be interpolated by the rasterizer.
out vec3 vertexColor;

void main(void)
{
    // Our positions are already in clip space.
    gl_Position = vec4(in_Position, 1.0);

    // Pass the vertex color to the rasterizer.
    vertexColor = in_Color;
}
