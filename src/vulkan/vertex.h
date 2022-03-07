/**
 * \file vertex.h
 * \brief Code relative to vertices that is used on the GPU.
 */
#pragma once

#include <vulkan/vulkan.h>
#include "../math/linear-algebra.h"

/**
 * \struct HxfVertex
 * \brief Represent a single vertex on the GPU.
 */
typedef struct HxfVertex {
    HxfVec3 pos; ///< The position of the vertex
    HxfVec3 color; ///< The color of the vertex
} HxfVertex;

/**
 * \brief Get the binding description of a HxfVertex.
 */
VkVertexInputBindingDescription hxfVertexGetBindingDescription();

/**
 * \brief Get the attribute descriptions of a HxfVertex.
 * 
 * Note: the return value must be freed.
 * 
 * \return A 2 dimensional array of VkVertexInputAttributeDescription that will need to be freed.
 */
VkVertexInputAttributeDescription * hxfVertexGetAttributeDescriptions();