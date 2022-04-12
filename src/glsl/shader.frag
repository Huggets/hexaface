#version 450

// Coordinate for the texture
layout(location = 0) in vec2 inTexelCoordinates;

// The textures
layout(binding = 1) uniform sampler2D texelSampler;

// The color that is rendered
layout(location = 0) out vec4 outColor;

void main() {
    // Get the texture color
    outColor = vec4(texture(texelSampler, inTexelCoordinates).rgb, 1.0);
}