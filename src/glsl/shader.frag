#version 450

layout(location = 0) in vec3 inColor;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 lightColor;
} ubo;

void main() {
    outColor = vec4(inColor * ubo.lightColor, 1.0);
}