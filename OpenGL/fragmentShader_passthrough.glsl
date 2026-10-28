
#version 330 core

// Interpolated vertex color from the rasterizer.
in vec3 vertexColor;

// Output fragment color.
layout(location = 0) out vec4 fragmentColor;

// Controlled by the C++ application.
// 0 = solid color
// 1 = interpolated vertex colors
uniform bool useVertexColors;

void main(void)
{
    if (useVertexColors)
    {
        // STEP 3:
        // The GPU automatically interpolates the colors
        // from the three vertices using barycentric weights.
        fragmentColor = vec4(vertexColor, 1.0);
    }
    else
    {
        // STEP 2:
        // Render the entire triangle with one solid color.
        fragmentColor = vec4(
            0.49,
            0.87,
            0.99,
            1.0
        );
    }
}
