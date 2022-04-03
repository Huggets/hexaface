#version 450

// In data
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inOffset;
layout(location = 2) in vec3 inColor;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 lightColor;
} ubo;

// Out data
layout(location = 0) out vec3 outColor;


void main() {
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition + inOffset, 1.0);
    outColor = inColor;
}