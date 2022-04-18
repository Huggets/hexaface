#include "world.h"
#include "hxf.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define WORLD_PIECE_FILE_SIZE HXF_WORLD_PIECE_CUBE_COUNT * sizeof(uint32_t)

#define WORLD_INFO_YAW_SIZE sizeof(float)
#define WORLD_INFO_YAW_OFFSET 0

#define WORLD_INFO_PITCH_SIZE sizeof(float)
#define WORLD_INFO_PITCH_OFFSET WORLD_INFO_YAW_OFFSET + WORLD_INFO_YAW_SIZE

#define WORLD_INFO_POSITON_SIZE sizeof(HxfVec3)
#define WORLD_INFO_POSITION_OFFSET WORLD_INFO_PITCH_OFFSET + WORLD_INFO_PITCH_SIZE

#define WORLD_INFO_FILE_SIZE WORLD_INFO_POSITION_OFFSET + WORLD_INFO_POSITON_SIZE

/**
 * @brief Compare two HxfIvec3 and return true if they have the same values.
 *
 * @param a HxfIvec3*
 * @param b HxfIvec3*
 *
 * @return 1 if true, 0 otherwise.
 */
static int worldPieceMapCompareKey(const void* a, const void* b) {
    const HxfIvec3* const operandA = (HxfIvec3*)a;
    const HxfIvec3* const operandB = (HxfIvec3*)b;

    return operandA->x == operandB->x && operandA->y == operandB->y && operandA->z == operandB->z;
}

static void generateWorldPiece(HxfWorldPiece* restrict worldPiece) {
    for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
        for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
            for (int y = 0; y != 2; y++) {
                worldPiece->cubes[x][y][z] = 3;
            }
        }
    }
}

/**
 * @brief Load a world piece from file, from its position.
 *
 * @param position The position of the world piece inside the world.
 *
 * @return A pointer to the world piece that is loaded.
 */
static HxfWorldPiece* loadWorldPiece(const char* restrict worldDirectory, const HxfIvec3* position) {
    HxfWorldPiece* worldPiece = hxfCalloc(1, sizeof(HxfWorldPiece));
    worldPiece->position = *position;

    // Get the filename

    int offset = strlen(worldDirectory);
    char* filename = hxfMalloc(sizeof(char) * (offset + 31)); // 10 characters for each world coordinates
    memcpy(filename, worldDirectory, offset);
    sprintf(filename + offset, "/%i_%i_%i", position->x, position->y, position->z);

    // Open the file for reading

    char filecontent[WORLD_PIECE_FILE_SIZE] = { 0 };
    FILE* file = fopen(filename, "rb");

    // If the file exist load it
    if (file) {
        fread(filecontent, sizeof(char), WORLD_PIECE_FILE_SIZE, file);
        fclose(file);

        uint32_t* cubes = (uint32_t*)filecontent;

        for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
            for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
                for (int y = 0; y != HXF_WORLD_PIECE_SIZE; y++) {
                    worldPiece->cubes[x][y][z] = cubes[x * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE + y * HXF_WORLD_PIECE_SIZE + z];
                }
            }
        }
    }
    // Else generate the world piece
    else {
        generateWorldPiece(worldPiece);
    }

    hxfFree(filename);

    return worldPiece;
}

static void saveWorldPiece(const HxfWorldPiece* restrict worldPiece, const char* restrict worldDirectory) {
    // Get the filename

    int offset = strlen(worldDirectory);
    char* filename = hxfMalloc(sizeof(char) * (offset + 31)); // 10 characters for each world coordinates
    memcpy(filename, worldDirectory, offset);
    sprintf(filename + offset, "/%i_%i_%i", worldPiece->position.x, worldPiece->position.y, worldPiece->position.z);

    char filecontent[WORLD_PIECE_FILE_SIZE] = { 0 };
    FILE* file = fopen(filename, "wb");
    if (file == NULL) { HXF_FATAL("Could not save the world"); }

    uint32_t* cubes = (uint32_t*)filecontent;

    for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
        for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
            for (int y = 0; y != HXF_WORLD_PIECE_SIZE; y++) {
                cubes[x * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE + y * HXF_WORLD_PIECE_SIZE + z] = worldPiece->cubes[x][y][z];
            }
        }
    }

    fwrite(filecontent, sizeof(char), sizeof(filecontent), file);

    fclose(file);
    hxfFree(filename);
}

static void loadWorldInfo(HxfWorldSaveData* restrict data) {
    int offset = strlen(data->world->directoryPath);
    char* filename = hxfMalloc(sizeof(char) * (offset + 31)); // 10 characters for each world coordinates
    memcpy(filename, data->world->directoryPath, offset);
    memcpy(filename + offset, "/info", 6);

    char filecontent[WORLD_INFO_FILE_SIZE];

    FILE* file = fopen(filename, "rb");
    if (file) {
        fread(filecontent, sizeof(char), WORLD_INFO_FILE_SIZE, file);
        fclose(file);

        float* fileYaw = (float*)(filecontent + WORLD_INFO_YAW_OFFSET);
        float* filePitch = (float*)(filecontent + WORLD_INFO_PITCH_OFFSET);
        HxfVec3* filePosition = (HxfVec3*)(filecontent + WORLD_INFO_POSITION_OFFSET);

        *data->cameraPitch = *filePitch;
        *data->cameraYaw = *fileYaw;
        *data->cameraPosition = *filePosition;
    }
    else {
        *data->cameraPitch = 0;
        *data->cameraYaw = 0;
        data->cameraPosition->x = 0;
        data->cameraPosition->y = 0;
        data->cameraPosition->z = 0;
    }

    hxfFree(filename);
}

static void saveWorldInfo(HxfWorldSaveData* restrict data) {
    int offset = strlen(data->world->directoryPath);
    char* filename = hxfMalloc(sizeof(char) * (offset + 31)); // 10 characters for each world coordinates
    memcpy(filename, data->world->directoryPath, offset);
    memcpy(filename + offset, "/info", 6);

    char filecontent[WORLD_INFO_FILE_SIZE];

    FILE* file = fopen(filename, "wb");
    if (file == NULL) { HXF_FATAL("Could not save the world"); }

    float* fileYaw = (float*)(filecontent + WORLD_INFO_YAW_OFFSET);
    float* pitchFile = (float*)(filecontent + WORLD_INFO_PITCH_OFFSET);
    HxfVec3* positionFile = (HxfVec3*)(filecontent + WORLD_INFO_POSITION_OFFSET);

    *fileYaw = *data->cameraYaw;
    *pitchFile = *data->cameraPitch;
    *positionFile = *data->cameraPosition;

    fwrite(filecontent, sizeof(char), WORLD_INFO_FILE_SIZE, file);
    fclose(file);
    hxfFree(filename);
}

HxfIvec3 hxfWorldGetPiecePositionF(const HxfVec3* restrict globalPosition) {
    HxfIvec3 localPosition;

    if (globalPosition->x < 0) {
        localPosition.x = (int32_t)floorf(globalPosition->x / (float)HXF_WORLD_PIECE_SIZE);
    }
    else {
        localPosition.x = (int32_t)(globalPosition->x / (float)HXF_WORLD_PIECE_SIZE);
    }

    if (globalPosition->y < 0) {
        localPosition.y = (int32_t)floorf(globalPosition->y / (float)HXF_WORLD_PIECE_SIZE);
    }
    else {
        localPosition.y = (int32_t)(globalPosition->y / (float)HXF_WORLD_PIECE_SIZE);
    }

    if (globalPosition->z < 0) {
        localPosition.z = (int32_t)floorf(globalPosition->z / (float)HXF_WORLD_PIECE_SIZE);
    }
    else {
        localPosition.z = (int32_t)(globalPosition->z / (float)HXF_WORLD_PIECE_SIZE);
    }

    return localPosition;
}

HxfIvec3 hxfWorldGetPiecePositionI(const HxfIvec3* restrict globalPosition) {
    HxfIvec3 localPosition;

    if (globalPosition->x < 0) {
        localPosition.x = (globalPosition->x + 1) / HXF_WORLD_PIECE_SIZE - 1;
    }
    else {
        localPosition.x = globalPosition->x / HXF_WORLD_PIECE_SIZE;
    }

    if (globalPosition->y < 0) {
        localPosition.y = (globalPosition->y + 1) / HXF_WORLD_PIECE_SIZE - 1;
    }
    else {
        localPosition.y = globalPosition->y / HXF_WORLD_PIECE_SIZE;
    }

    if (globalPosition->z < 0) {
        localPosition.z = (globalPosition->z + 1) / HXF_WORLD_PIECE_SIZE - 1;
    }
    else {
        localPosition.z = globalPosition->z / HXF_WORLD_PIECE_SIZE;
    }

    return localPosition;
}

HxfIvec3 hxfWorldGetLocalPosition(const HxfIvec3* restrict globalPosition) {
    HxfIvec3 localPosition = {
        globalPosition->x % HXF_WORLD_PIECE_SIZE,
        globalPosition->y % HXF_WORLD_PIECE_SIZE,
        globalPosition->z % HXF_WORLD_PIECE_SIZE,
    };

    if (localPosition.x < 0) localPosition.x += 16;
    if (localPosition.y < 0) localPosition.y += 16;
    if (localPosition.z < 0) localPosition.z += 16;

    return localPosition;
}

void hxfWorldLoad(HxfWorldSaveData* restrict data) {
    loadWorldInfo(data);

    HxfMap* const cubesMap = &data->world->pieces;
    cubesMap->compareKey = worldPieceMapCompareKey;

    const HxfIvec3 worldPiecePosition = hxfWorldGetPiecePositionF(data->cameraPosition);
    const int32_t minX = worldPiecePosition.x - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t maxX = worldPiecePosition.x + HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t minZ = worldPiecePosition.z - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t maxZ = worldPiecePosition.z + HXF_HORIZONTAL_VIEW_DISTANCE / 2;

    for (int32_t x = minX; x != maxX; x++) {
        for (int32_t z = minZ; z != maxZ; z++) {
            HxfIvec3 pos = { x, 0, z };
            HxfWorldPiece* piece = loadWorldPiece(data->world->directoryPath, &pos);
            hxfMapSet(cubesMap, &piece->position, piece);
        }
    }
}

void hxfWorldSave(HxfWorldSaveData* restrict data) {
    saveWorldInfo(data);

    // Free the map along with the elements’ value that was previously allocated.

    HxfMapElement* iterator = data->world->pieces.start;

    while (iterator != NULL) {
        saveWorldPiece((HxfWorldPiece*)iterator->value, data->world->directoryPath);
        // TODO correct hxfMapRemove to avoid a freeze when exiting the application.
        // The bug seems to happened when free the iterator value then remove the map element.
        // So for now, the map is manually cleaned.

        HxfMapElement* next = iterator->next;
        void* value = iterator->value;
        hxfFree(iterator);
        hxfFree(value);
        iterator = next;
    }
}

int hxfWorldUpdatePiece(HxfWorld* restrict world, const HxfVec3* restrict position) {
    int wasUpdated = 0;

    const HxfIvec3 worldPiecePosition = hxfWorldGetPiecePositionF(position);
    const int32_t minX = worldPiecePosition.x - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t maxX = worldPiecePosition.x + HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t minZ = worldPiecePosition.z - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t maxZ = worldPiecePosition.z + HXF_HORIZONTAL_VIEW_DISTANCE / 2;

    // Remove the world piece that are out of the view distance

    HxfMapElement* iterator = world->pieces.start;
    void* toRemove[HXF_HORIZONTAL_VIEW_DISTANCE * HXF_VERTICAL_VIEW_DISTANCE * 2];
    int i = 0;

    while (iterator != NULL) {
        HxfIvec3* position = (HxfIvec3*)iterator->key;

        if (position->x < minX || position->x >= maxX || position->z < minZ || position->z >= maxZ) {
            toRemove[i] = iterator->key;
            i++;
        }
        iterator = iterator->next;
    }

    if (i != 0) {
        wasUpdated = 1;
        for (int j = 0; j != i; j++) {
            void* value = hxfMapGet(&world->pieces, toRemove[j])->value;
            hxfMapRemove(&world->pieces, toRemove[j]);
            hxfFree(value);
        }
    }

    // Add the new world piece

    for (int32_t x = minX; x != maxX; x++) {
        for (int32_t z = minZ; z != maxZ; z++) {
            const HxfIvec3 position = { x, 0, z };
            if (hxfMapGet(&world->pieces, &position) == NULL) {
                HxfWorldPiece* piece = loadWorldPiece(world->directoryPath, &position);
                hxfMapSet(&world->pieces, &piece->position, piece);
                wasUpdated = 1;
            }
        }
    }

    return wasUpdated;
}
