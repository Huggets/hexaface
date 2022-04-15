#include "world.h"
#include "hxf.h"
#include <stdio.h>
#include <stdint.h>

#define CUBES_SIZE sizeof(uint32_t) * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH
#define CAMERA_POSITION_SIZE sizeof(HxfVec3)
#define CAMERA_YAW_SIZE sizeof(float)
#define CAMERA_OFFSET_SIZE sizeof(float)

#define CAMERA_POSITION_OFFSET 0
#define CAMERA_YAW_OFFSET CAMERA_POSITION_OFFSET + CAMERA_POSITION_SIZE
#define CAMERA_PITCH_OFFSET CAMERA_YAW_OFFSET + CAMERA_YAW_SIZE
#define CUBES_OFFSET CAMERA_POSITION_OFFSET + CAMERA_POSITION_SIZE

#define FILE_SIZE CUBES_OFFSET + CUBES_SIZE

/**
 * @brief Create a world file.
 *
 * It create a world file that already contains cubes.
 *
 * @param filename The name of the file.
 */
static void createWorldFile(const char* restrict filename) {
    // Create the file

    FILE* file = fopen(filename, "w");

    if (!file) {
        HXF_FATAL("Could not create world file");
    }

    // Add default cubes to the world

    char filecontent[FILE_SIZE] = { 0 };
    uint32_t* fileCubes = (uint32_t*)(filecontent + CUBES_OFFSET);
    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
            // A layer of stone
            for (int y = 0; y != 2; y++) {
                fileCubes[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + y * HXF_WORLD_LENGTH + z] = 3;
            }
            // A layer of dirt
            fileCubes[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + 2 * HXF_WORLD_LENGTH + z] = 2;
            // And a layer of grass
            fileCubes[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + 3 * HXF_WORLD_LENGTH + z] = 1;
        }
    }

    // Write to file then close the file

    fwrite(filecontent, sizeof(char), FILE_SIZE, file);

    fclose(file);
}

void hxfWorldLoad(const char* restrict filename, HxfWorldSaveData* data) {
    // Open the world file.
    // If the file does not exist, it create a new world file.

    FILE* file = fopen(filename, "rb");

    if (!file) {
        createWorldFile(filename);

        file = fopen(filename, "rb");
        if (!file) {
            HXF_FATAL("Could not open the world file\n");
        }
    }

    // Get the content of the file

    char filecontent[FILE_SIZE];
    fread(filecontent, sizeof(char), FILE_SIZE, file);

    fclose(file);

    // Get the world data
    uint32_t* fileCubes = (uint32_t*)(filecontent + CUBES_OFFSET);
    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int y = 0; y != HXF_WORLD_LENGTH; y++) {
            for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
                data->world->cubes[x][y][z] = fileCubes[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + y * HXF_WORLD_LENGTH + z];
            }
        }
    }

    // Get the player position

    HxfVec3* fileCameraPosition = (HxfVec3*)(filecontent + CAMERA_POSITION_OFFSET);
    float* fileCameraYaw = (float*)(filecontent + CAMERA_YAW_OFFSET);
    float* fileCameraPitch = (float*)(filecontent + CAMERA_PITCH_OFFSET);
    *data->cameraPosition = *fileCameraPosition;
    *data->cameraYaw = *fileCameraYaw;
    *data->cameraPitch = *fileCameraPitch;
}

void hxfWorldSave(const char* restrict filename, HxfWorldSaveData* data) {
    FILE* file = fopen(filename, "w");

    if (!file) {
        HXF_MSG_ERROR("Could not open file to save the world");
        exit(EXIT_FAILURE);
    }

    char filecontent[FILE_SIZE];

    // Write the world to the file.

    uint32_t* fileCubes = (uint32_t*)(filecontent + CUBES_OFFSET);
    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int y = 0; y != HXF_WORLD_LENGTH; y++) {
            for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
                fileCubes[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + y * HXF_WORLD_LENGTH + z] = data->world->cubes[x][y][z];
            }
        }
    }

    // Write the camera data

    HxfVec3* fileCameraPosition = (HxfVec3*)(filecontent + CAMERA_POSITION_OFFSET);
    float* fileCameraYaw = (float*)(filecontent + CAMERA_YAW_OFFSET);
    float* fileCameraPitch = (float*)(filecontent + CAMERA_PITCH_OFFSET);
    *fileCameraPosition = *data->cameraPosition;
    *fileCameraYaw = *data->cameraYaw;
    *fileCameraPitch = *data->cameraPitch;

    fwrite(filecontent, sizeof(char), FILE_SIZE, file);

    fclose(file);
}
