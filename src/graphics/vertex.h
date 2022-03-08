/**
 * \file vertex.h
 * \brief Code relative to vertices that is used on the GPU.
 */
#pragma once

#include <vulkan/vulkan.h>
#include "../math/linear-algebra.h"

#define HXF_VERTEX_BINDING_DESCRIPTION_COUNT 2
#define HXF_VERTEX_ATTRIBUTE_DESCRIPTION_COUNT 3

typedef struct HxfVertexInstanceData {
    HxfVec3 offset;
} HxfVertexInstanceData;

/**
 * \struct HxfVertex
 * \brief Represent a single vertex on the GPU.
 */
typedef struct HxfVertex {
    HxfVec3 position; ///< The position of the vertex
    HxfVec3 color; ///< The color of the vertex
} HxfVertex;

/**
 * \brief Get the binding description of a HxfVertex.
 */
VkVertexInputBindingDescription * hxfVertexGetBindingDescriptions();

/**
 * \brief Get the attribute descriptions of a HxfVertex.
 * 
 * Note: the return value must be freed.
 * 
 * \return A 2 dimensional array of VkVertexInputAttributeDescription that will need to be freed.
 */
VkVertexInputAttributeDescription * hxfVertexGetAttributeDescriptions();