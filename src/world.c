#include "world.h"
#include "hxf.h"
#include <stdio.h>
#include <stdint.h>

#define CUBE_COUNT HXF_WORLD_LENGTH * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH

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
        HXF_MSG_ERROR("Could not create world file");
        exit(EXIT_FAILURE);
    }

    // Add default cubes to the world

    uint32_t filecontent[CUBE_COUNT] = { 0 };
    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
            // A layer of dirt
            for (int y = 0; y != 4; y++) {
                filecontent[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + y * HXF_WORLD_LENGTH + z] = 2;
            }
            // A layer of grass on top of it
            filecontent[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + 4 * HXF_WORLD_LENGTH + z] = 1;
        }
    }

    // Write to file then close the file

    fwrite(filecontent, sizeof(uint32_t), CUBE_COUNT, file);

    fclose(file);
}

void hxfWorldLoad(const char* restrict filename, HxfWorld* restrict world) {
    // Open the world file.
    // If the file does not exist, it create a new world file.
    
    FILE* file = fopen(filename, "rb");

    if (!file) {
        createWorldFile(filename);

        file = fopen(filename, "rb");
        if (!file) {
            HXF_MSG_ERROR("Could not open the world file\n");
            exit(EXIT_FAILURE);
        }
    }

    // Get the content of the file

    uint32_t filecontent[CUBE_COUNT];
    fread(filecontent, sizeof(uint32_t), CUBE_COUNT, file);

    fclose(file);

    // Convert the file content to world data

    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int y = 0; y != HXF_WORLD_LENGTH; y++) {
            for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
                world->cubes[x][y][z] = filecontent[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + y * HXF_WORLD_LENGTH + z];
            }
        }
    }
}

void hxfWorldSave(const char* restrict filename, const HxfWorld* restrict world) {
    FILE* file = fopen(filename, "w");

    if (!file) {
        HXF_MSG_ERROR("Could not open file to save the world");
        exit(EXIT_FAILURE);
    }

    // Write the world to the file.

    uint32_t filecontent[CUBE_COUNT];
    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int y = 0; y != HXF_WORLD_LENGTH; y++) {
            for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
                filecontent[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + y * HXF_WORLD_LENGTH + z] = world->cubes[x][y][z];
            }
        }
    }

    fwrite(filecontent, sizeof(uint32_t), CUBE_COUNT, file);

    fclose(file);
}
