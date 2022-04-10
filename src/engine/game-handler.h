#pragma once

#include "../world.h"
#include "../camera.h"
#include "vulkan.h"

typedef struct HxfGameData {
    /**
     * @brief The graphics engine
     */
    HxfEngine* engine;
    /**
     * @brief The camera where the scene is viewed by the player.
     */
    HxfCamera camera;
    /**
     * @brief The world that is made of cubes.
     */
    HxfWorld world;
    /**
     * @brief The texture index of the cube that will be placed.
     */
    uint32_t cubeSelector;
} HxfGameData;

/**
 * @brief Initialize the game.
 *
 * @param app A pointer to the application that hold the game.
 */
void hxfInitGame(HxfGameData* restrict game);

/**
 * @brief Run a single game frame.
 *
 * @param game A pointer to the game.
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