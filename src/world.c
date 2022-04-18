// #include "world.h"
// #include "hxf.h"
// #include <stdio.h>
// #include <stdint.h>

// #define CAMERA_POSITION_SIZE sizeof(HxfVec3)
// #define CAMERA_YAW_SIZE sizeof(float)
// #define CAMERA_OFFSET_SIZE sizeof(float)
// #define CUBES_SIZE sizeof(uint32_t) * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE

// #define CAMERA_POSITION_OFFSET 0
// #define CAMERA_YAW_OFFSET CAMERA_POSITION_OFFSET + CAMERA_POSITION_SIZE
// #define CAMERA_PITCH_OFFSET CAMERA_YAW_OFFSET + CAMERA_YAW_SIZE
// #define CUBES_OFFSET CAMERA_PITCH_OFFSET + CAMERA_OFFSET_SIZE

// #define FILE_SIZE CUBES_OFFSET + CUBES_SIZE

// /**
//  * @brief Create a world file.
//  *
//  * It create a world file that already contains cubes.
//  *
//  * @param filename The name of the file.
//  */
// static void createWorldFile(const char* restrict filename) {
//     // Create the file

//     FILE* file = fopen(filename, "w");

//     if (!file) {
//         HXF_FATAL("Could not create world file");
//     }

//     // Add default cubes to the world

//     char filecontent[FILE_SIZE] = { 0 };
//     uint32_t* fileCubes = (uint32_t*)(filecontent + CUBES_OFFSET);
//     for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
//         for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
//             // A layer of stone
//             for (int y = 0; y != 2; y++) {
//                 fileCubes[x * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE + y * HXF_WORLD_PIECE_SIZE + z] = 3;
//             }
//             // A layer of dirt
//             fileCubes[x * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE + 2 * HXF_WORLD_PIECE_SIZE + z] = 2;
//             // And a layer of grass
//             fileCubes[x * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE + 3 * HXF_WORLD_PIECE_SIZE + z] = 1;
//         }
//     }

//     // Write to file then close the file

//     fwrite(filecontent, sizeof(char), FILE_SIZE, file);

//     fclose(file);
// }

// void hxfWorldLoad(const char* restrict filename, HxfWorldSaveData* data) {
//     // Open the world file.
//     // If the file does not exist, it create a new world file.

//     FILE* file = fopen(filename, "rb");

//     if (!file) {
//         createWorldFile(filename);

//         file = fopen(filename, "rb");
//         if (!file) {
//             HXF_FATAL("Could not open the world file\n");
//         }
//     }

//     // Get the content of the file

//     char filecontent[FILE_SIZE];
//     fread(filecontent, sizeof(char), FILE_SIZE, file);

//     fclose(file);

//     // Get the world data
//     uint32_t* fileCubes = (uint32_t*)(filecontent + CUBES_OFFSET);
//     for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
//         for (int y = 0; y != HXF_WORLD_PIECE_SIZE; y++) {
//             for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
//                 data->world->piece.cubes[x][y][z] = fileCubes[x * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE + y * HXF_WORLD_PIECE_SIZE + z];
//             }
//         }
//     }

//     // Get the player position

//     HxfVec3* fileCameraPosition = (HxfVec3*)(filecontent + CAMERA_POSITION_OFFSET);
//     float* fileCameraYaw = (float*)(filecontent + CAMERA_YAW_OFFSET);
//     float* fileCameraPitch = (float*)(filecontent + CAMERA_PITCH_OFFSET);
//     *data->cameraPosition = *fileCameraPosition;
//     *data->cameraYaw = *fileCameraYaw;
//     *data->cameraPitch = *fileCameraPitch;
// }

// void hxfWorldSave(const char* restrict filename, HxfWorldSaveData* data) {
//     FILE* file = fopen(filename, "w");

//     if (!file) {
//         HXF_MSG_ERROR("Could not open file to save the world");
//         exit(EXIT_FAILURE);
//     }

//     char filecontent[FILE_SIZE];

//     // Write the world to the file.

//     uint32_t* fileCubes = (uint32_t*)(filecontent + CUBES_OFFSET);
//     for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
//         for (int y = 0; y != HXF_WORLD_PIECE_SIZE; y++) {
//             for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
//                 fileCubes[x * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE + y * HXF_WORLD_PIECE_SIZE + z] = data->world->piece.cubes[x][y][z];
//             }
//         }
//     }

//     // Write the camera data

//     HxfVec3* fileCameraPosition = (HxfVec3*)(filecontent + CAMERA_POSITION_OFFSET);
//     float* fileCameraYaw = (float*)(filecontent + CAMERA_YAW_OFFSET);
//     float* fileCameraPitch = (float*)(filecontent + CAMERA_PITCH_OFFSET);
//     *fileCameraPosition = *data->cameraPosition;
//     *fileCameraYaw = *data->cameraYaw;
//     *fileCameraPitch = *data->cameraPitch;

//     fwrite(filecontent, sizeof(char), FILE_SIZE, file);

//     fclose(file);
// }

#include "world.h"
#include "hxf.h"
#include <math.h>

/**
 * @brief The function used with the world pieces map to compare two key.
 *
 * @param a An array of 3 int
 * @param b
 * @return int
 */
static int worldPieceMapCompareKey(const void* a, const void* b) {
    const HxfIvec3* const operandA = (HxfIvec3*)a;
    const HxfIvec3* const operandB = (HxfIvec3*)b;

    return operandA->x == operandB->x && operandA->y == operandB->y && operandA->z == operandB->z;
}

void hxfWorldLoad(const char* restrict filename, HxfWorldSaveData* restrict data) {
    data->cameraPitch = 0;
    data->cameraYaw = 0;
    data->cameraPosition->x = -1;
    data->cameraPosition->y = 3;
    data->cameraPosition->z = 0;

    HxfMap* const restrict cubesMap = &data->world->pieces;
    HxfWorldPiece* piece;
    cubesMap->compareKey = worldPieceMapCompareKey;

    piece = hxfMalloc(sizeof(HxfWorldPiece));
    piece->position.x = 0;
    piece->position.y = 0;
    piece->position.z = 0;
    for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
        for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
            for (int y = 0; y != 3; y++) {
                piece->cubes[x][y][z] = 2;
            }
            for (int y = 3; y != HXF_WORLD_PIECE_SIZE; y++) {
                piece->cubes[x][y][z] = 0;
            }
        }
    }
    hxfMapSet(cubesMap, &piece->position, piece);

    piece = hxfMalloc(sizeof(HxfWorldPiece));
    piece->position.x = 1;
    piece->position.y = 0;
    piece->position.z = 0;
    for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
        for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
            for (int y = 0; y != 5; y++) {
                piece->cubes[x][y][z] = 3;
            }
            for (int y = 5; y != HXF_WORLD_PIECE_SIZE; y++) {
                piece->cubes[x][y][z] = 0;
            }
        }
    }
    hxfMapSet(cubesMap, &piece->position, piece);

    piece = hxfMalloc(sizeof(HxfWorldPiece));
    piece->position.x = -1;
    piece->position.y = 0;
    piece->position.z = 0;
    for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
        for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
            for (int y = 0; y != 5; y++) {
                piece->cubes[x][y][z] = 3;
            }
            for (int y = 5; y != HXF_WORLD_PIECE_SIZE; y++) {
                piece->cubes[x][y][z] = 0;
            }
        }
    }
    hxfMapSet(cubesMap, &piece->position, piece);

    piece = hxfMalloc(sizeof(HxfWorldPiece));
    piece->position.x = 1;
    piece->position.y = 0;
    piece->position.z = 1;
    for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
        for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
            for (int y = 0; y != 5; y++) {
                piece->cubes[x][y][z] = 3;
            }
            for (int y = 5; y != HXF_WORLD_PIECE_SIZE; y++) {
                piece->cubes[x][y][z] = 0;
            }
        }
    }
    hxfMapSet(cubesMap, &piece->position, piece);

    piece = hxfMalloc(sizeof(HxfWorldPiece));
    piece->position.x = 0;
    piece->position.y = 0;
    piece->position.z = -1;
    for (int x = 0; x != HXF_WORLD_PIECE_SIZE; x++) {
        for (int z = 0; z != HXF_WORLD_PIECE_SIZE; z++) {
            for (int y = 0; y != 4; y++) {
                piece->cubes[x][y][z] = 3;
            }
            for (int y = 5; y != HXF_WORLD_PIECE_SIZE; y++) {
                piece->cubes[x][y][z] = 0;
            }
        }
    }
    hxfMapSet(cubesMap, &piece->position, piece);
}

void hxfWorldSave(const char* restrict filename, HxfWorldSaveData* restrict data) {
    // Free the map along with the elements’ value that was previously allocated.

    HxfMapElement* iterator = data->world->pieces.start;

    while (iterator != NULL) {
        // TODO correct hxfMapRemove to avoid a freeze when exiting the application.
        // The bug seems to happened when free the iterator value then remove the map element.
        // So for now, the map is manually cleaned.

        HxfMapElement* next = iterator->next;
        hxfFree(iterator->value);
        hxfFree(iterator);
        iterator = next;
    }
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
