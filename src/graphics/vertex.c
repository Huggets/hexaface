#include "vertex.h"
#include "../hxf.h"

VkVertexInputBindingDescription * hxfVertexGetBindingDescriptions() {
    VkVertexInputBindingDescription * bindingDescription =
        hxfMalloc(sizeof(VkVertexInputBindingDescription) * HXF_VERTEX_BINDING_DESCRIPTION_COUNT);

    bindingDescription[0].binding = 0;
    bindingDescription[0].stride = sizeof(HxfVertex);
    bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    bindingDescription[1].binding = 1;
    bindingDescription[1].stride = sizeof(HxfVertexInstanceData);
    bindingDescription[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

VkVertexInputAttributeDescription * hxfVertexGetAttributeDescriptions() {
    VkVertexInputAttributeDescription * attributeDescriptions =
        hxfMalloc(sizeof(VkVertexInputAttributeDescription) * HXF_VERTEX_ATTRIBUTE_DESCRIPTION_COUNT);

    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(HxfVertex, position);

    // Color
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(HxfVertex, color);

    // Offset
    attributeDescriptions[2].binding = 1;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(HxfVertexInstanceData, offset);

    return attributeDescriptions;
}