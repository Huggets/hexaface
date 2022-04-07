#version 450

// In data
layout(location = 0) in vec3 inPosition; // Vertex position
layout(location = 2) in vec3 inOffset;   // Vertex position offset
layout(location = 3) in vec3 inColor;    // Vertex color

layout(binding = 0) uniform UBO {
    mat4 model;      // Model
    mat4 view;       // View
    mat4 projection; // Projection
} ubo;

layout(location = 0) out vec3 outColor;

void main() {
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition + inOffset, 1.0);
    outColor = inColor;
}