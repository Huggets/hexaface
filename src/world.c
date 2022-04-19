#include "world.h"
#include "hxf.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Offset and size of data inside the files */

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

/**
 * @brief Generate a single world piece.
 *
 * @param worldPiece A pointer to a world piece.
 */
static void generateWorldPiece(HxfWorldPiece* restrict worldPiece) {
    for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
        for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
            for (int y = 0; y != 2; y++) {
                worldPiece->cubes[x][y][z] = 2; // Dirt
            }
            worldPiece->cubes[x][2][z] = 1; // Grass
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
    // Create a new world piece filled with air and with the given position

    HxfWorldPiece* worldPiece = hxfCalloc(1, sizeof(HxfWorldPiece));
    worldPiece->position = *position;

    // Get the filename

    int size = strlen(worldDirectory);
    char* filename = hxfMalloc(sizeof(char) * (size + 31)); // 10 characters for each world coordinates
    memcpy(filename, worldDirectory, size);
    sprintf(filename + size, "/%i_%i_%i", position->x, position->y, position->z);

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

    int size = strlen(worldDirectory);
    char* filename = hxfMalloc(sizeof(char) * (size + 31)); // 10 characters for each world coordinates
    memcpy(filename, worldDirectory, size);
    sprintf(filename + size, "/%i_%i_%i", worldPiece->position.x, worldPiece->position.y, worldPiece->position.z);

    // Write the world piece to the buffer

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

    // Write the buffer to the file

    fwrite(filecontent, sizeof(char), sizeof(filecontent), file);

    fclose(file);
    hxfFree(filename);
}

static void loadWorldInfo(HxfWorldSaveData* restrict data) {
    // Get the filename

    int size = strlen(data->world->directoryPath);
    char* filename = hxfMalloc(sizeof(char) * (size + 31)); // 10 characters for each world coordinates
    memcpy(filename, data->world->directoryPath, size);
    memcpy(filename + size, "/info", 6);

    char filecontent[WORLD_INFO_FILE_SIZE];

    FILE* file = fopen(filename, "rb");
    if (file) { // If the file exists load it
        fread(filecontent, sizeof(char), WORLD_INFO_FILE_SIZE, file);
        fclose(file);

        float* const fileYaw = (float*)(filecontent + WORLD_INFO_YAW_OFFSET);
        float* const filePitch = (float*)(filecontent + WORLD_INFO_PITCH_OFFSET);
        HxfVec3* const filePosition = (HxfVec3*)(filecontent + WORLD_INFO_POSITION_OFFSET);

        *data->cameraPitch = *filePitch;
        *data->cameraYaw = *fileYaw;
        *data->cameraPosition = *filePosition;
    }
    else { // Else use default values
        *data->cameraPitch = 0;
        *data->cameraYaw = 0;
        data->cameraPosition->x = 0;
        data->cameraPosition->y = 0;
        data->cameraPosition->z = 0;
    }

    hxfFree(filename);
}

static void saveWorldInfo(HxfWorldSaveData* restrict data) {
    // Get the filename

    int size = strlen(data->world->directoryPath);
    char* filename = hxfMalloc(sizeof(char) * (size + 31)); // 10 characters for each world coordinates
    memcpy(filename, data->world->directoryPath, size);
    memcpy(filename + size, "/info", 6);

    // Fill the buffer with the world info

    char filecontent[WORLD_INFO_FILE_SIZE];

    FILE* file = fopen(filename, "wb");
    if (file == NULL) { HXF_FATAL("Could not save the world"); }

    float* fileYaw = (float*)(filecontent + WORLD_INFO_YAW_OFFSET);
    float* pitchFile = (float*)(filecontent + WORLD_INFO_PITCH_OFFSET);
    HxfVec3* positionFile = (HxfVec3*)(filecontent + WORLD_INFO_POSITION_OFFSET);

    *fileYaw = *data->cameraYaw;
    *pitchFile = *data->cameraPitch;
    *positionFile = *data->cameraPosition;

    // Write the buffer to the file

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

    // Initialize the world pieces

    HxfMap* const cubesMap = &data->world->pieces;
    cubesMap->compareKey = worldPieceMapCompareKey;

    // Load the world pieces around the camera position according to the view distance

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

    const char* const directoryPath = data->world->directoryPath;
    HxfMap* const map = &data->world->pieces;
    HxfMapElement* start = map->start;
    while (start != NULL) {
        HxfWorldPiece* value = start->value; ///< The piece that will be saved and freed 

         // Save the piece on the disk
        saveWorldPiece(value, directoryPath);

        // Delete the map element and free the piece
        hxfMapRemove(map, start->key);
        hxfFree(value);

        start = map->start;
    }
}

int hxfWorldUpdatePiece(HxfWorld* restrict world, const HxfVec3* restrict position) {
    /**
     * @brief Set to one when a world piece was removed.
     *
     * No need to set this to one when a piece is added because this mean a piece was also removed.
     */
    int wasUpdated = 0;
    HxfMap* const worldPieces = &world->pieces;
    const char* const worldDirectory = world->directoryPath;

    // Get the minimum and maximum world piece coordinate that will be loaded.
    // Minimum is exclusive and maximum inclusive.

    const HxfIvec3 worldPiecePosition = hxfWorldGetPiecePositionF(position);
    const int32_t minX = worldPiecePosition.x - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t maxX = worldPiecePosition.x + HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t minZ = worldPiecePosition.z - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t maxZ = worldPiecePosition.z + HXF_HORIZONTAL_VIEW_DISTANCE / 2;

    // Remove the world piece that are out of the view distance

    HxfMapElement* iterator = worldPieces->start;
    HxfMapElement* toRemove[HXF_HORIZONTAL_VIEW_DISTANCE * HXF_VERTICAL_VIEW_DISTANCE * 2]; // The size of two layers to be sure there is enough place

    // Find the pieces that need to be removed

    int i = 0;
    while (iterator != NULL) {
        HxfIvec3* position = (HxfIvec3*)iterator->key;

        if (position->x < minX || position->x >= maxX || position->z < minZ || position->z >= maxZ) {
            toRemove[i] = iterator;
            i++;
        }
        iterator = iterator->next;
    }

    // If i != 0, then there are pieces that need to be removed

    if (i != 0) {
        wasUpdated = 1;
        for (int j = 0; j != i; j++) {
            void* value = toRemove[j]->value;
            hxfMapRemove(worldPieces, toRemove[j]->key);
            hxfFree(value);
        }
    }

    // Add the new world piece

    for (int32_t x = minX; x != maxX; x++) {
        for (int32_t z = minZ; z != maxZ; z++) {
            // Load the world piece if it is not loaded

            const HxfIvec3 position = { x, 0, z };
            if (hxfMapGet(worldPieces, &position) == NULL) {
                HxfWorldPiece* piece = loadWorldPiece(worldDirectory, &position);
                hxfMapSet(worldPieces, &piece->position, piece);
            }
        }
    }

    return wasUpdated;
}
