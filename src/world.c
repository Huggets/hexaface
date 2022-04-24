#include "world.h"
#include "hxf.h"
#include <math.h>
#include <string.h>

#include <stdio.h>
#include <time.h>

/* Offset and size of data inside the files */

#define WORLD_PIECE_FILE_SIZE HXF_WORLD_PIECE_CUBE_COUNT * sizeof(uint32_t)

#define WORLD_INFO_YAW_OFFSET 0
#define WORLD_INFO_YAW_SIZE sizeof(float)
#define WORLD_INFO_PITCH_OFFSET (WORLD_INFO_YAW_OFFSET + WORLD_INFO_YAW_SIZE)
#define WORLD_INFO_PITCH_SIZE sizeof(float)
#define WORLD_INFO_POSITION_OFFSET (WORLD_INFO_PITCH_OFFSET + WORLD_INFO_PITCH_SIZE)
#define WORLD_INFO_POSITON_SIZE sizeof(HxfVec3)

#define WORLD_INFO_FILE_SIZE (WORLD_INFO_POSITION_OFFSET + WORLD_INFO_POSITON_SIZE)

/**
 * @brief The hash function used with the world pieces hash map.
 *
 * TODO finish
 *
 * @param key HxfUvec3 (the position)
 *
 * @return The hash.
 */
static uint32_t hashPosition(const void* restrict key) {
    uint32_t hash = 0;
    const uint32_t horizontal = HXF_WORLD_WIDTH;
    const uint32_t horizontal2 = horizontal * horizontal;
    const uint32_t vertical = HXF_WORLD_HEIGHT;

    hash += (((HxfUvec3*)key)->y * horizontal2) % (vertical * horizontal2);
    hash += (((HxfUvec3*)key)->x * horizontal) % (horizontal2);
    hash += ((HxfUvec3*)key)->z % horizontal;

    return hash;
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

static void saveWorldPiece(HxfWorldPiece* restrict worldPiece, const char* restrict worldDirectory) {
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
    hxfFree(worldPiece);
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

HxfIvec3 hxfWorldPieceGetPositionF(const HxfVec3* restrict globalPosition) {
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

HxfIvec3 hxfWorldPieceGetPositionI(const HxfIvec3* restrict globalPosition) {
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

void hxfWorldNormalizePosition(const HxfIvec3* restrict startCorner, const HxfIvec3* restrict in, HxfUvec3* restrict out) {
    out->x = in->x - startCorner->x;
    out->y = in->y - startCorner->y;
    out->z = in->z - startCorner->z;
}

void hxfWorldLoad(HxfWorldSaveData* restrict data) {
    loadWorldInfo(data);

    // Initialize the world pieces

    HxfHashMap* const worldPiecesMap = &data->world->pieces;
    worldPiecesMap->hash = hashPosition;
    worldPiecesMap->table = hxfCalloc(1, HXF_WORLD_PIECE_MAP_COUNT * sizeof(HxfWorldPiece*));

    // Load the world pieces around the camera position according to the view distance

    const HxfIvec3 worldPiecePosition = hxfWorldPieceGetPositionF(data->cameraPosition);
    data->world->inStartCorner.x = worldPiecePosition.x - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    data->world->inStartCorner.y = 0;
    data->world->inStartCorner.z = worldPiecePosition.z - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    data->world->inEndCorner.x = worldPiecePosition.x + HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    data->world->inEndCorner.y = 0;
    data->world->inEndCorner.z = worldPiecePosition.z + HXF_HORIZONTAL_VIEW_DISTANCE / 2;

    data->world->outStartCorner.x = data->world->inStartCorner.x - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    data->world->outStartCorner.y = 0;
    data->world->outStartCorner.z = data->world->inStartCorner.z - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    data->world->outEndCorner.x = data->world->inEndCorner.x + HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    data->world->outEndCorner.y = 0;
    data->world->outEndCorner.z = data->world->inEndCorner.z + HXF_HORIZONTAL_VIEW_DISTANCE / 2;

    for (int32_t x = data->world->inStartCorner.x; x != data->world->inEndCorner.x; x++) {
        for (int32_t z = data->world->inStartCorner.z; z != data->world->inEndCorner.z; z++) {
            HxfIvec3 pos = { x, 0, z };
            HxfUvec3 normPos;
            hxfWorldNormalizePosition(&data->world->outStartCorner, &pos, &normPos);
            HxfWorldPiece* piece = loadWorldPiece(data->world->directoryPath, &pos);

            hxfHashMapPut(worldPiecesMap, &normPos, piece);
        }
    }
}

void hxfWorldSave(HxfWorldSaveData* restrict data) {
    saveWorldInfo(data);

    HxfWorldPiece** const worldPieces = (HxfWorldPiece**)data->world->pieces.table;
    const char* const directoryPath = data->world->directoryPath;

    // Save all the pieces in the world pieces map as they are all loaded in the world.

    for (uint32_t i = 0; i != HXF_WORLD_PIECE_MAP_COUNT; i++) {
        HxfWorldPiece* const restrict worldPiece = worldPieces[i];
        if (worldPiece != NULL) {
            saveWorldPiece(worldPieces[i], directoryPath);
        }
    }

    hxfFree(worldPieces);
}

int hxfWorldUpdatePiece(HxfWorld* restrict world, const HxfVec3* restrict position) {
    int updated = 0;
    HxfHashMap* const restrict worldPiecesMap = &world->pieces;
    const char* const restrict worldDirectory = world->directoryPath;

    // Get the minimum and maximum world piece coordinate that will be loaded.
    // Minimum is inclusive and maximum exclusive.

    const HxfIvec3 worldPiecePosition = hxfWorldPieceGetPositionF(position);
    const int32_t minX = worldPiecePosition.x - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t minZ = worldPiecePosition.z - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t maxX = worldPiecePosition.x + HXF_HORIZONTAL_VIEW_DISTANCE / 2;
    const int32_t maxZ = worldPiecePosition.z + HXF_HORIZONTAL_VIEW_DISTANCE / 2;

    int needRearrange =
        minX < world->outStartCorner.x || maxX >= world->outEndCorner.x ||
        minZ < world->outStartCorner.z || maxZ >= world->outEndCorner.z;

    // Remove the world piece that are out of the view distance and keep the others.
    // Those that are kept are associated with a new hash, due to the the fact that
    // worldPiecePosition has changed.

    static int32_t prevDiffX = 0;
    static int32_t prevDiffZ = 0;
    const int32_t diffX = minX - world->inStartCorner.x;
    const int32_t diffZ = minZ - world->inStartCorner.z;

    if (needRearrange) {
        updated = 1;

        HxfIvec3 oldStartCorner = world->inStartCorner;
        HxfIvec3 oldOutStartCorner = world->outStartCorner;
        HxfIvec3 oldEndCorner = world->inEndCorner;
        HxfHashMap oldMap = *worldPiecesMap;
        worldPiecesMap->table = hxfCalloc(1, HXF_WORLD_PIECE_MAP_COUNT * sizeof(HxfWorldPiece*));

        world->inStartCorner.x = minX;
        world->inStartCorner.z = minZ;
        world->inEndCorner.x = maxX;
        world->inEndCorner.z = maxZ;
        world->outStartCorner.x = minX - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
        world->outStartCorner.z = minZ - HXF_HORIZONTAL_VIEW_DISTANCE / 2;
        world->outEndCorner.x = maxX + HXF_HORIZONTAL_VIEW_DISTANCE / 2;
        world->outEndCorner.z = maxZ + HXF_HORIZONTAL_VIEW_DISTANCE / 2;

        // Get all the world piece needed.
        // Get those already loaded and load the others.
        for (int32_t x = minX; x != maxX; x++) {
            for (int32_t z = minZ; z != maxZ; z++) {
                HxfIvec3 position = { x, 0, z };
                HxfUvec3 normalizedPosition;
                HxfWorldPiece* worldPiece;
                if (x >= oldStartCorner.x && x < oldEndCorner.x && z >= oldStartCorner.z && z < oldEndCorner.z) {
                    hxfWorldNormalizePosition(&oldOutStartCorner, &position, &normalizedPosition);
                    uint32_t hash = hashPosition(&normalizedPosition);
                    worldPiece = hxfHashMapGetFromHash(&oldMap, hash);
                    hxfHashMapPutFromHash(&oldMap, hash, NULL);
                }
                else {
                    worldPiece = loadWorldPiece(worldDirectory, &position);
                }
                hxfWorldNormalizePosition(&world->outStartCorner, &position, &normalizedPosition);
                hxfHashMapPut(worldPiecesMap, &normalizedPosition, worldPiece);
            }
        }

        for (int i = 0; i != HXF_WORLD_PIECE_MAP_COUNT; i++) {
            hxfFree(oldMap.table[i]);
            hxfHashMapPutFromHash(&oldMap, i, NULL);
        }

        hxfFree(oldMap.table);
    }
    else if ((prevDiffX != diffX && diffX != 0) || (prevDiffZ != diffZ && diffZ != 0)) {
        updated = 1;

        for (int32_t x = minX; x != maxX; x++) {
            for (int32_t z = minZ; z != maxZ; z++) {
                HxfIvec3 position = { x, 0, z };
                HxfUvec3 normalizedPosition;
                hxfWorldNormalizePosition(&world->outStartCorner, &position, &normalizedPosition);
                uint32_t hash = hashPosition(&normalizedPosition);
                if (hxfHashMapGetFromHash(worldPiecesMap, hash) == NULL) {
                    hxfHashMapPutFromHash(worldPiecesMap, hash, loadWorldPiece(worldDirectory, &position));
                }
            }
        }


        prevDiffX = diffX;
        prevDiffZ = diffZ;
    }

    return updated;
}
