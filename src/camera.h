/**
 * \file camera.h
 * \brief Function relative to the camera.
 */
#pragma once

#include "math/linear-algebra.h"
#include "world.h"

/**
 * \struct HxfCamera
 * TODO
 */
typedef struct HxfCamera {
    HxfVec3 up; ///< Define the up direction.
    HxfVec3 front; ///< Define the looking direction.
    HxfVec3 position; ///< The position of the camera.
    HxfVec3 direction; ///< The direction of the view matrix
    float pitch; ///< The pitch of the camera.
    float yaw; ///< The yaw of the camera.
    HxfIvec3 pointedCube; ///< The coordinate of the cube the camera is pointing to.
    int isPointingToCube; ///< Set to 1 if the camera is pointing to a cube, 0 otherwise.
    HxfIvec3 nearPointedCube; ///< The cube that is just before the pointed cube. Especially used to place a cube.
} HxfCamera;

/**
 * @brief Update the pointed cube.
 * 
 * @param camera The camera that is aiming at the cube.
 * @param world The world where the cubes are.
 */
void hxfUpdatePointedCube(HxfCamera* restrict camera, const HxfWorld* restrict world);