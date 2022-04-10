#include "camera.h"

#include <math.h>
#include <stdio.h>

void hxfUpdatePointedCube(HxfCamera* restrict camera, const HxfWorld* restrict world) {
    const int maxPointingDistance = 5; // Maximum distance at which the block can be selected
    const int precision = 4;

    int i = 1;
    int cubeNotFound = 1;

    while (cubeNotFound && i != maxPointingDistance * precision) {
        // Select the next cube
        const float div = (float)i / (float) precision;

        const HxfVec3 forward = { camera->direction.x * div, camera->direction.y * div, camera->direction.z * div };
        HxfVec3 cubeFloatPosition = camera->position;
        cubeFloatPosition = hxfVec3Add(&cubeFloatPosition, &forward);

        // Round its position to the smallest integer.
        // e.g. 2.3 -> 2, 2.7-> 2, -2.1 -> -3

        HxfIvec3 cubePosition;
        if (cubeFloatPosition.x < 0) {
            cubePosition.x = (int)floorf(cubeFloatPosition.x);
        }
        else {
            cubePosition.x = (int)cubeFloatPosition.x;
        }
        if (cubeFloatPosition.y < 0) {
            cubePosition.y = (int)floorf(cubeFloatPosition.y);
        }
        else {
            cubePosition.y = (int)cubeFloatPosition.y;
        }
        if (cubeFloatPosition.z < 0) {
            cubePosition.z = (int)floorf(cubeFloatPosition.z);
        }
        else {
            cubePosition.z = (int)cubeFloatPosition.z;
        }

        // If pointing inside the world

        if (cubePosition.x >= 0 && cubePosition.x < HXF_WORLD_LENGTH
            && cubePosition.y >= 0 && cubePosition.y < HXF_WORLD_LENGTH
            && cubePosition.z >= 0 && cubePosition.z < HXF_WORLD_LENGTH) {

            if (world->cubes[cubePosition.x][cubePosition.y][cubePosition.z] != 0) {
                // Cube found

                camera->pointedCube = cubePosition;
                camera->isPointingToCube = 1;
                cubeNotFound = 0;
            }
        }
        i++;
    }

    if (cubeNotFound) {
        camera->isPointingToCube = 0;
    }
}
