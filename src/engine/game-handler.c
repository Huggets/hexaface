#include "game-handler.h"

static const char WORLD_DIRECTORY[] = "/world"; ///< The path to the world file

/**
 * @brief Append a new cube’s face that will be drawn.
 *
 * @param face A pointer to an array of faces where the face will be added.
 * @param position The position of the new face in the world.
 * @param textureIndex The texture index of the face.
 * @param index The index of the new face. It is incremented after the cube is added.
 */
static void addDrawnFace(HxfCubeInstanceData* restrict faces, const HxfVec3* restrict position, uint32_t textureIndex, size_t* index) {
    faces->position = *position;
    faces->textureIndex = textureIndex;
    (*index)++;
}

/**
 * @brief Update the drawing data’s faces to draw only those that are needed.
 *
 * @param game A pointer to game that own the drawing data.
 */
static void updateDrawnFaces(HxfGameData* restrict game) {
    HxfDrawingData* const drawingData = &game->graphics->drawingData;

    // Reset all the faces. No faces are drawn.

    drawingData->faceTopCount = 0;
    drawingData->faceBottomCount = 0;
    drawingData->faceFrontCount = 0;
    drawingData->faceBackCount = 0;
    drawingData->faceRightCount = 0;
    drawingData->faceLeftCount = 0;

    // Select the faces that are not hidden by other cubes.
    HxfMapElement* iterator = game->world.pieces.start;
    while (iterator != NULL) { // For each world piece.
        for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
            for (int y = 0; y != HXF_WORLD_PIECE_SIZE; y++) {
                for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
                    HxfWorldPiece* const worldPiece = (HxfWorldPiece*)iterator->value;
                    HxfIvec3* const worldPiecePosition = (HxfIvec3*)iterator->key;

                    const uint32_t textureId = worldPiece->cubes[x][y][z];
                    const HxfVec3 position = { x + worldPiecePosition->x * HXF_WORLD_PIECE_SIZE, y + worldPiecePosition->y * HXF_WORLD_PIECE_SIZE, z + worldPiecePosition->z * HXF_WORLD_PIECE_SIZE };

                    if (textureId != 0) {
                        if ((x != HXF_WORLD_PIECE_SIZE - 1 && worldPiece->cubes[x + 1][y][z] == 0)
                            || x == HXF_WORLD_PIECE_SIZE - 1) {
                            size_t* index = &drawingData->faceRightCount;
                            addDrawnFace(&drawingData->cubeInstances[HXF_FACES_RIGHT_OFFSET + *index], &position, textureId, index);
                        }
                        if ((x != 0 && worldPiece->cubes[x - 1][y][z] == 0)
                            || x == 0) {
                            size_t* index = &drawingData->faceLeftCount;
                            addDrawnFace(&drawingData->cubeInstances[HXF_FACES_LEFT_OFFSET + *index], &position, textureId, index);
                        }
                        if ((y != HXF_WORLD_PIECE_SIZE - 1 && worldPiece->cubes[x][y + 1][z] == 0)
                            || y == HXF_WORLD_PIECE_SIZE - 1) {
                            size_t* index = &drawingData->faceTopCount;
                            addDrawnFace(&drawingData->cubeInstances[HXF_FACES_TOP_OFFSET + *index], &position, textureId, index);
                        }
                        if ((y != 0 && worldPiece->cubes[x][y - 1][z] == 0)
                            || y == 0) {
                            size_t* index = &drawingData->faceBottomCount;
                            addDrawnFace(&drawingData->cubeInstances[HXF_FACES_BOTTOM_OFFSET + *index], &position, textureId, index);
                        }
                        if ((z != HXF_WORLD_PIECE_SIZE - 1 && worldPiece->cubes[x][y][z + 1] == 0)
                            || z == HXF_WORLD_PIECE_SIZE - 1) {
                            size_t* index = &drawingData->faceFrontCount;
                            addDrawnFace(&drawingData->cubeInstances[HXF_FACES_FRONT_OFFSET + *index], &position, textureId, index);
                        }
                        if ((z != 0 && worldPiece->cubes[x][y][z - 1] == 0)
                            || z == 0) {
                            size_t* index = &drawingData->faceBackCount;
                            addDrawnFace(&drawingData->cubeInstances[HXF_FACES_BACK_OFFSET + *index], &position, textureId, index);
                        }
                    }
                }
            }
        }
        iterator = iterator->next;
    }
}

void hxfGameInit(HxfGameData* restrict game) {
    game->cubeSelector = 1;

    char** worldpath = &game->world.directoryPath;
    *worldpath = hxfMalloc(sizeof(char) * (strlen(game->appdataDirectory) + sizeof(WORLD_DIRECTORY)));
    strcpy(*worldpath, game->appdataDirectory);
    strcat(*worldpath, WORLD_DIRECTORY);

    HxfWorldSaveData savedData = {
        .world = &game->world,
        .cameraPosition = &game->camera.position,
        .cameraYaw = &game->camera.yaw,
        .cameraPitch = &game->camera.pitch
    };
    hxfWorldLoad(&savedData);

    updateDrawnFaces(game); // We need to define which cubes’ faces will be drawn

}

void hxfGameStop(HxfGameData* restrict game) {
    HxfWorldSaveData savedData = {
        .world = &game->world,
        .cameraPosition = &game->camera.position,
        .cameraYaw = &game->camera.yaw,
        .cameraPitch = &game->camera.pitch
    };
    hxfWorldSave(&savedData);

    hxfFree(game->world.directoryPath);
}

void hxfGameFrame(HxfGameData* restrict game) {
    // Update the pointer
    hxfUpdatePointedCube(&game->camera, &game->world);

    // Update the world’s pieces
    if (hxfWorldUpdatePiece(&game->world, &game->camera.position)) {
        // Update the faces to draw if pieces were removed/added
        updateDrawnFaces(game);
        hxfGraphicsUpdateCubeBuffer(game->graphics);
    }
}

void hxfReplaceCube(HxfGameData* restrict game, const HxfIvec3* restrict position, uint32_t textureIndex) {
    // Replace the cube if it is inside a world piece that is loaded

    HxfIvec3 cubeRelativePosition = hxfWorldGetPiecePositionI(position);
    HxfMapElement* worldPieceElement = hxfMapGet(&game->world.pieces, &cubeRelativePosition);

    if (worldPieceElement != NULL) {
        HxfIvec3 localPosition = hxfWorldGetLocalPosition(position);
        ((HxfWorldPiece*)worldPieceElement->value)->cubes[localPosition.x][localPosition.y][localPosition.z] = textureIndex;

        updateDrawnFaces(game);
        hxfGraphicsUpdateCubeBuffer(game->graphics);
    }
}