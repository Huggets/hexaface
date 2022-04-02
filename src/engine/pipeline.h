#pragma once

#include "vulkan.h"

/**
 * @brief Create the graphics pipeline.
 * 
 * It includes the render pass
 *
 * @param engine A pointer to a HxfEngine that will own it.
 */
void createGraphicsPipeline(HxfEngine* restrict engine);