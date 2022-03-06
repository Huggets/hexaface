#include "vertex.h"
#include "../hxf.h"

VkVertexInputBindingDescription hxfVertexGetBindingDescription() {
    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(HxfVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

VkVertexInputAttributeDescription * hxfVertexGetAttributeDescriptions() {
    VkVertexInputAttributeDescription * attributeDescriptions = hxfMalloc(sizeof(VkVertexInputAttributeDescription) * 2);

    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(HxfVertex, pos);

    // Color
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(HxfVertex, color);

    return attributeDescriptions;
}