#include "game-handler.h"

/**
 * @brief Append a new cube’s face that will be drawn.
 *
 * @param face A pointer to an array of faces where the face will be added.
 * @param position The position of the new face in the world.
 * @param textureIndex The texture index of the face.
 * @param index The index of the new face. It is incremented after the cube is added.
 */
static void addDrawnFace(HxfCubeData* restrict face, const HxfVec3* restrict position, uint32_t textureIndex, size_t* index) {
    face[*index].cubePosition = *position;
    face[*index].textureIndex = textureIndex;
    (*index)++;
}

/**
 * @brief Update the drawing data’s faces to draw only those that are needed.
 *
 * @param game A pointer to game that own the drawing data.
 */
static void updateDrawnFaces(HxfGameData* restrict game) {
    HxfDrawingData* const drawingData = &game->engine->drawingData;

    // Reset all the faces. No faces are drawn.

    drawingData->faceTopCount = 0;
    drawingData->faceBottomCount = 0;
    drawingData->faceFrontCount = 0;
    drawingData->faceBackCount = 0;
    drawingData->faceRightCount = 0;
    drawingData->faceLeftCount = 0;

    // Select the faces that are not hidden by other cubes.

    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int y = 0; y != HXF_WORLD_LENGTH; y++) {
            for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
                const uint32_t textureId = game->world.cubes[x][y][z];
                const HxfVec3 position = { x, y, z };

                if (textureId != 0) {
                    if ((x != HXF_WORLD_LENGTH - 1 && game->world.cubes[x + 1][y][z] == 0)
                        || x == HXF_WORLD_LENGTH - 1) {
                        addDrawnFace(drawingData->faces[HXF_FACES_RIGHT], &position, textureId, &drawingData->faceRightCount);
                    }
                    if ((x != 0 && game->world.cubes[x - 1][y][z] == 0)
                        || x == 0) {
                        addDrawnFace(drawingData->faces[HXF_FACES_LEFT], &position, textureId, &drawingData->faceLeftCount);
                    }
                    if ((y != HXF_WORLD_LENGTH - 1 && game->world.cubes[x][y + 1][z] == 0)
                        || y == HXF_WORLD_LENGTH - 1) {
                        addDrawnFace(drawingData->faces[HXF_FACES_TOP], &position, textureId, &drawingData->faceTopCount);
                    }
                    if ((y != 0 && game->world.cubes[x][y - 1][z] == 0)
                        || y == 0) {
                        addDrawnFace(drawingData->faces[HXF_FACES_BOTTOM], &position, textureId, &drawingData->faceBottomCount);
                    }
                    if ((z != HXF_WORLD_LENGTH - 1 && game->world.cubes[x][y][z + 1] == 0)
                        || z == HXF_WORLD_LENGTH - 1) {
                        addDrawnFace(drawingData->faces[HXF_FACES_FRONT], &position, textureId, &drawingData->faceFrontCount);
                    }
                    if ((z != 0 && game->world.cubes[x][y][z - 1] == 0)
                        || z == 0) {
                        addDrawnFace(drawingData->faces[HXF_FACES_BACK], &position, textureId, &drawingData->faceBackCount);
                    }
                }
            }
        }
    }
}

void hxfInitGame(HxfGameData* restrict game) {
    game->cubeSelector = 1;

    hxfWorldLoad("world", &game->world);
    updateDrawnFaces(game); // We need to define which cubes’ faces will be drawn
}

void hxfStopGame(HxfGameData* restrict game) {
    hxfWorldSave("world", &game->world);
}

void hxfGameFrame(HxfGameData* restrict game) {
    hxfUpdatePointedCube(&game->camera, &game->world);
}

void hxfReplaceCube(HxfGameData* restrict game, const HxfIvec3* restrict position, uint32_t textureIndex) {
    // If the cube is outside of the world then it does nothing.

    if (position->x < 0 || position->x >= HXF_WORLD_LENGTH || position->z < 0 || position->z >= HXF_WORLD_LENGTH || position->y < 0 || position->y >= HXF_WORLD_LENGTH) {
        return;
    }

    // Otherwise replace the cube (it just means change its texture),
    // and update the drawn faces

    game->world.cubes[position->x][position->y][position->z] = textureIndex;
    updateDrawnFaces(game);
    hxfEngineUpdateCubeBuffer(game->engine);
}