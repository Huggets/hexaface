#include "game-handler.h"

static void setCube(HxfCubeData* restrict cubes, HxfDrawingData* restrict drawingData, const HxfVec3* restrict position, uint32_t textureId, size_t* index) {
    cubes[*index].cubePosition = *position;
    cubes[*index].cubeColor = drawingData->textures[textureId];
    (*index)++;
}

static void updateDrawingFaces(HxfGameData* restrict game) {
    HxfDrawingData* const drawingData = &game->engine->drawingData;
    drawingData->faceTopCount = 0;
    drawingData->faceBottomCount = 0;
    drawingData->faceFrontCount = 0;
    drawingData->faceBackCount = 0;
    drawingData->faceRightCount = 0;
    drawingData->faceLeftCount = 0;

    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int y = 0; y != HXF_WORLD_LENGTH; y++) {
            for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
                const uint32_t textureId = game->world.cubes[x][y][z];
                const HxfVec3 position = { x, y, z };

                if (textureId != 0) {
                    if ((x != HXF_WORLD_LENGTH - 1 && game->world.cubes[x + 1][y][z] == 0)
                        || x == HXF_WORLD_LENGTH - 1) {
                        setCube(drawingData->faces[4], drawingData, &position, textureId, &drawingData->faceRightCount);
                    }
                    if ((x != 0 && game->world.cubes[x - 1][y][z] == 0)
                        || x == 0) {
                        setCube(drawingData->faces[5], drawingData, &position, textureId, &drawingData->faceLeftCount);
                    }
                    if ((y != HXF_WORLD_LENGTH - 1 && game->world.cubes[x][y + 1][z] == 0)
                        || y == HXF_WORLD_LENGTH - 1) {
                        setCube(drawingData->faces[0], drawingData, &position, textureId, &drawingData->faceTopCount);
                    }
                    if ((y != 0 && game->world.cubes[x][y - 1][z] == 0)
                        || y == 0) {
                        setCube(drawingData->faces[2], drawingData, &position, textureId, &drawingData->faceBottomCount);
                    }
                    if ((z != HXF_WORLD_LENGTH - 1 && game->world.cubes[x][y][z + 1] == 0)
                        || z == HXF_WORLD_LENGTH - 1) {
                        setCube(drawingData->faces[1], drawingData, &position, textureId, &drawingData->faceBackCount);
                    }
                    if ((z != 0 && game->world.cubes[x][y][z - 1] == 0)
                        || z == 0) {
                        setCube(drawingData->faces[3], drawingData, &position, textureId, &drawingData->faceFrontCount);
                    }
                }
            }
        }
    }
}

static void initWorld(HxfGameData* restrict game) {
    // Initialize the cubes

    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int y = 0; y != 3; y++) {
            for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
                game->world.cubes[x][y][z] = 1;
            }
        }
    }
    game->world.cubes[0][3][0] = 2;
    game->world.cubes[0][4][0] = 2;
    game->world.cubes[0][3][1] = 2;
}

void hxfInitGame(HxfGameData* restrict game) {
    hxfWorldLoad("world", &game->world);

    // initWorld(game);
    updateDrawingFaces(game);
    game->cubeSelector = 1;
}

void hxfStopGame(HxfGameData* restrict game) {
    hxfWorldSave("world", &game->world);
}

void hxfGameFrame(HxfGameData* restrict game) {
    hxfUpdatePointedCube(&game->camera, &game->world);
}

void hxfReplaceCube(HxfGameData* restrict game, const HxfIvec3* restrict position, uint32_t textureIndex) {
    if (position->x < 0 || position->x >= HXF_WORLD_LENGTH || position->z < 0 || position->z >= HXF_WORLD_LENGTH || position->y < 0 || position->y >= HXF_WORLD_LENGTH) {
        return;
    }

    game->world.cubes[position->x][position->y][position->z] = textureIndex;
    updateDrawingFaces(game);
    hxfEngineUpdateCubeBuffer(game->engine);
}