#include "game-handler.h"

static const char WORLD_DIRECTORY[] = "/world"; ///< The path to the world file

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

        hxfGraphicsUpdateCubeBuffer(game->graphics);
    }
}