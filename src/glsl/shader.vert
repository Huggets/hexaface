#version 450

// In data
layout(location = 0) in vec3 inPosition; // Vertex position
layout(location = 1) in vec3 inNormal;   // Vertex normal
layout(location = 2) in vec3 inOffset;   // Vertex position offset
layout(location = 3) in vec3 inColor;    // Vertex color

layout(binding = 0) uniform UBO {
    mat4 model;      // Model
    mat4 view;       // View
    mat4 projection; // Projection
    vec3 lightPosition; // Position of the light
    vec3 lightColor; // Color of the light
} ubo;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outFragPosition;

void main() {
    vec4 vertexPosition = vec4(inPosition + inOffset, 1.0);
    
    gl_Position = ubo.projection * ubo.view * ubo.model * vertexPosition;
    outColor = inColor;
    outNormal = inNormal;
    outFragPosition = vec3(ubo.model * vertexPosition);
}