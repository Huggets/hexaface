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
#define HXF_HORIZONTAL_VIEW_DISTANCE 8 // Must be even
#define HXF_VERTICAL_VIEW_DISTANCE 1   // Must be even

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
    char* directoryPath; ///< The path to the directory of the world.
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
 * @brief Get the world local position from the global position.
 *
 * @param globalPosition The global position.
 *
 * @return The world piece position.
 */
HxfIvec3 hxfWorldGetPiecePositionF(const HxfVec3* restrict globalPosition);

/**
 * @brief Get the world local position from the global position.
 *
 * @param globalPosition The global position.
 *
 * @return The world piece position.
 */
HxfIvec3 hxfWorldGetPiecePositionI(const HxfIvec3* restrict globalPosition);

/**
 * @brief Get the location of a cube inside its world piece
 *
 * @param globalPosition The global position
 *
 * @return The local position.
 */
HxfIvec3 hxfWorldGetLocalPosition(const HxfIvec3* restrict globalPosition);


/**
 * @brief Load a world from a disk.
 *
 * If the file does not exist, it creates a new default world file.
 *
 * @param data A pointer to the data that will be load.
 */
void hxfWorldLoad(HxfWorldSaveData* restrict data);

/**
 * @brief Save a world to a disk.
 *
 * @param data A pointer to the data that will be saved.
 */
void hxfWorldSave(HxfWorldSaveData* restrict data);

/**
 * @brief Update the world’s pieces loaded according to the view distance and the
 * camera position.
 *
 * @param world The world to update.
 * @param position The camera position.
 *
 * @return 1 if a world piece was updated (removed or added), 0 otherwise.
 */
int hxfWorldUpdatePiece(HxfWorld* restrict world, const HxfVec3* restrict position);