#version 450

// Width of textures.png
#define TEXTURE_WIDTH 96.0
// Height of texture.png
#define TEXTURE_HEIGHT 80.0

// Position of the vertex
layout(location = 0) in vec3 inPosition;
// Offset that is added to the position of the vertex
layout(location = 1) in vec3 inOffset;
// texel coordinates for the texture
layout(location = 2) in vec2 inTexelCoordinates;
// the texture index
layout(location = 3) in uint textureIndex;

// The model-view-projection matrices
layout(binding = 0) uniform UBO {
    mat4 model;      // Model
    mat4 view;       // View
    mat4 projection; // Projection
} ubo;

// the texel coordinate for the texture that will be used by the fragment shader
layout(location = 0) out vec2 outTexelCoordinates;

// Main

void main() {
    // The vertex position is:
    // The position + the offset that is then transformed by the MVP matrices to be in clip space
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition + inOffset, 1.0);

    // Set the texel coordinate that correspond to the texture (via texture index)
    outTexelCoordinates = vec2(inTexelCoordinates.x, inTexelCoordinates.y + textureIndex * 16.0 / TEXTURE_HEIGHT);
}