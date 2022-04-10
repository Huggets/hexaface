#include "world.h"
#include "hxf.h"
#include <stdio.h>
#include <stdint.h>

#define CUBE_COUNT HXF_WORLD_LENGTH * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH

static void createFile(const char* restrict filename) {
    FILE* file = fopen(filename, "w");

    if (!file) {
        HXF_MSG_ERROR("Could not create world file");
        exit(EXIT_FAILURE);
    }

    uint32_t filecontent[CUBE_COUNT] = { 0 };
    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
            for (int y = 0; y != 4; y++) {
                filecontent[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + y * HXF_WORLD_LENGTH + z] = 1;
            }
            filecontent[x * HXF_WORLD_LENGTH * HXF_WORLD_LENGTH + 4 * HXF_WORLD_LENGTH + z] = 2;
        }
    }
    fwrite(filecontent, sizeof(uint32_t), CUBE_COUNT, file);

    fclose(file);
}

void hxfWorldLoad(const char* restrict filename, HxfWorld* restrict world) {
    FILE* file = fopen(filename, "rb");

    // TODO This may overwrite an existing that failed to open

    if (!file) {
        createFile(filename);

        file = fopen(filename, "rb");
        if (!file) {
            HXF_MSG_ERROR("Could not open the world file\n");
            exit(EXIT_FAILURE);
        }
    }

    uint32_t filecontent[CUBE_COUNT];
    fread(filecontent, sizeof(uint32_t), CUBE_COUNT, file);

    fclose(file);

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
