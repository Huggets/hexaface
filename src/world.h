#pragma once

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
 * @brief Load a world from a file.
 * 
 * If the file does not exist, it creates a new default world file.
 *
 * @param filename The file’s name that will be loaded.
 * @param world A pointer to the world that will be created from the file.
 */
void hxfWorldLoad(const char* restrict filename, HxfWorld* restrict world);

/**
 * @brief Save a world to a file.
 *
 * @param filename The file’s name that will be saved.
 * @param world A pointer to the world that will be saved.
 */
void hxfWorldSave(const char* restrict filename, const HxfWorld* restrict world);