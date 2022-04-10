#pragma once

#include "math/linear-algebra.h"
#include <stdint.h>

#define HXF_WORLD_LENGTH 16

/**
 * @brief Represent a world that is made of cubes.
 */
typedef struct HxfWorld {
    /**
     * @brief An array containing all the cubes of the world.
     * 
     * The value is the texture index of the block.
     */
    uint32_t cubes[HXF_WORLD_LENGTH][HXF_WORLD_LENGTH][HXF_WORLD_LENGTH];
} HxfWorld;

void hxfReplaceCube(const HxfVec3* restrict position, uint32_t textureIndex);