#version 450

layout(location = 0) in vec2 inTexelCoordinates;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texelSampler;

void main() {
    outColor = vec4(texture(texelSampler, inTexelCoordinates).rgb, 1.0);
}