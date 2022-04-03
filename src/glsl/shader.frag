#version 450

layout(location = 0) in vec3 inColor;        // Fragment color
layout(location = 1) in vec3 inNormal;       // Fragment normal
layout(location = 2) in vec3 inFragPosition; // Fragment position

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UBO {
    mat4 model;         // Model
    mat4 view;          // View
    mat4 projection;    // Projection
    vec3 lightPosition; // Position of the light
    vec3 lightColor;    // Color of the light
} ubo;

void main() {
    vec3 lightDir = normalize(ubo.lightPosition - inFragPosition);

    float diff = max(dot(inNormal, lightDir), 0.0);
    vec3 diffuse = diff * ubo.lightColor;

    outColor = vec4((ubo.lightColor * 0.125 + diffuse) * inColor, 1.0);
}