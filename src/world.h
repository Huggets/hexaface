#pragma once

#include "math/linear-algebra.h"
#include "container/map.h"
#include <stdint.h>

/**
 * @brief The size of the single world’s piece.
 */
#define HXF_WORLD_PIECE_SIZE 16
/**
 * @brief The number of cube in a single world’s piece
 *
 */
#define HXF_WORLD_PIECE_CUBE_COUNT HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE * HXF_WORLD_PIECE_SIZE
#define HXF_HORIZONTAL_VIEW_DISTANCE 32
#define HXF_VERTICAL_VIEW_DISTANCE 4

/**
 * @brief A piece of the world.
 *
 * It’s a big cube of 16×16×16 cubes.
 */
typedef struct HxfWorldPiece {
    HxfIvec3 position; ///< The world piece position inside the world.
    uint32_t cubes[HXF_WORLD_PIECE_SIZE][HXF_WORLD_PIECE_SIZE][HXF_WORLD_PIECE_SIZE]; ///< The array of cubes of the world piece.
} HxfWorldPiece;

/**
 * @brief Represent a world that is made of cubes.
 */
typedef struct HxfWorld {
    HxfMap pieces;
} HxfWorld;

/**
 * @brief Arguments passed to hxfWorldLoad or hxfWorldSave
 */
typedef struct HxfWorldSavedData {
    HxfWorld* world;
    HxfVec3* cameraPosition;
    float* cameraYaw;
    float* cameraPitch;
} HxfWorldSaveData;

/**
 * @brief Load a world from a file.
 *
 * If the file does not exist, it creates a new default world file.
 *
 * @param filename The file’s name that will be loaded.
 * @param data A pointer to the data that will be load.
 */
void hxfWorldLoad(const char* restrict filename, HxfWorldSaveData* restrict data);

/**
 * @brief Save a world to a file.
 *
 * @param filename The file’s name that will be saved.
 * @param data A pointer to the data that will be saved.
 */
void hxfWorldSave(const char* restrict filename, HxfWorldSaveData* restrict data);

/**
 * @brief Get the local position of inside a world piece.
 *
 * @param globalPosition The global position.
 *
 * @return The local position.
 */
HxfIvec3 hxfWorldGetPiecePositionF(const HxfVec3* restrict globalPosition);

/**
 * @brief Get the local position of inside a world piece.
 *
 * @param globalPosition The global position.
 *
 * @return The local position.
 */
HxfIvec3 hxfWorldGetPiecePositionI(const HxfIvec3* restrict globalPosition);

HxfIvec3 hxfWorldGetLocalPosition(const HxfIvec3* restrict globalPosition);
