#pragma once

#include "../world.h"
#include "../camera.h"
#include "vulkan.h"

typedef struct HxfGameData {
    HxfEngine* engine;

    /**
     * @brief The camera where the scene is viewed by the player.
     */
    HxfCamera camera;

    /**
     * @brief The world that is made of cubes.
     */
    HxfWorld world;
} HxfGameData;

/**
 * @brief Initialize the game.
 * 
 * @param app The application that hold the game.
 */
void hxfInitGame(HxfGameData* restrict game);