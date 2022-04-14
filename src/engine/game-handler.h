#pragma once

#include "../world.h"
#include "../camera.h"
#include "graphics-handler.h"

typedef struct HxfGameData {
    const char* const appdataDirectory;

    HxfGraphicsHandler* graphics; ///< A reference to the graphics handler.
    HxfCamera camera; ///< The player’s camera.
    HxfWorld world; ///< The world that is made of cubes.
    uint32_t cubeSelector; ///< The texture index of the cube that will be placed.
} HxfGameData;

/**
 * @brief Initialize the game.
 */
void hxfGameInit(HxfGameData* restrict game);

/**
 * @brief Stop the game.
 */
void hxfGameStop(HxfGameData* restrict game);

/**
 * @brief Run a single game frame.
 */
void hxfGameFrame(HxfGameData* restrict game);

/**
 * @brief Replace the cube at the position by textureIndex.
 *
 * @param game The game where the cubes are.
 * @param position The position of the cube to replace.
 * @param textureIndex The texture index to set.
 */
void hxfReplaceCube(HxfGameData* restrict game, const HxfIvec3* restrict position, uint32_t textureIndex);