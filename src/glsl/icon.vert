#version 450

// Width of textures.png
#define TEXTURE_WIDTH 96.0
// Height of texture.png
#define TEXTURE_HEIGHT 80.0

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexelCoordinates;
layout(location = 2) in uint inTextureIndex;

layout(push_constant) uniform Push {
    uint windowWidth;
    uint windowHeight;
} push;

layout(location = 0) out vec2 outTexelCoordinates;

void main() {
    const vec2 pos = vec2(
        inPosition.x / (push.windowWidth / 2.0),
        inPosition.y / (push.windowHeight / 2.0)
    );
    gl_Position = vec4(pos, 0.0, 1.0);

    outTexelCoordinates = vec2(inTexelCoordinates.x, inTexelCoordinates.y + inTextureIndex * 16.0 / TEXTURE_HEIGHT);
}