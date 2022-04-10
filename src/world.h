#pragma once

#include <stdint.h>

#define HXF_WORLD_LENGTH 16

/**
 * @brief Represent a world that is made of cubes.
 */
typedef struct HxfWorld {
    /**
     * @brief An array containing all the cubes of the world.
     */
    uint32_t cubes[HXF_WORLD_LENGTH][HXF_WORLD_LENGTH][HXF_WORLD_LENGTH];
} HxfWorld;