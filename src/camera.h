/**
 * \file camera.h
 * \brief Function relative to the camera.
 */
#pragma once

#include "math/linear-algebra.h"

/**
 * \struct HxfCamera
 * TODO
 */
typedef struct HxfCamera {
    HxfVec3 up; ///< Define the up direction.
    HxfVec3 front; ///< Define the looking direction.
    HxfVec3 position; ///< The position of the camera.
    float pitch; ///< The pitch of the camera.
    float yaw; ///< The yaw of the camera.
} HxfCamera;