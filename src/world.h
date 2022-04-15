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

/**
 * @brief Arguments passed to hxfWorldLoad or hxfWorldSave
 */
typedef struct HxfWorldSavedData {
    HxfWorld* world;
    HxfVec3* cameraPosition;
    float* cameraYaw;
    float* cameraPitch;
} HxfWorldSaveData;

/**
 * @brief Load a world from a file.
 *
 * If the file does not exist, it creates a new default world file.
 *
 * @param filename The file’s name that will be loaded.
 * @param data A pointer to the data that will be load.
 */
void hxfWorldLoad(const char* restrict filename, HxfWorldSaveData* restrict data);

/**
 * @brief Save a world to a file.
 *
 * @param filename The file’s name that will be saved.
 * @param data A pointer to the data that will be saved.
 */
void hxfWorldSave(const char* restrict filename, HxfWorldSaveData* restrict data);