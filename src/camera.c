#include "camera.h"

#include <math.h>
#include <stdio.h>


void hxfUpdatePointedCube(HxfCamera* restrict camera, const HxfWorld* restrict world) {
    const int maxPointingDistance = 5; // Maximum distance at which the block can be selected
    const int precision = 50;

    int i = 1;
    int cubeNotFound = 1;

    const HxfVec3 forward = { camera->direction.x / (float)precision, camera->direction.y / (float)precision, camera->direction.z / (float)precision };

    HxfVec3 floatPosition = camera->position;
    HxfIvec3 nearCube = roundVector(&camera->position);

    while (cubeNotFound && i != maxPointingDistance * precision) {
        // Select the next cube

        floatPosition = hxfVec3Add(&floatPosition, &forward);

        HxfIvec3 intPosition = roundVector(&floatPosition);

        // If pointing inside the world

        if (intPosition.x >= 0 && intPosition.x < HXF_WORLD_LENGTH
            && intPosition.y >= 0 && intPosition.y < HXF_WORLD_LENGTH
            && intPosition.z >= 0 && intPosition.z < HXF_WORLD_LENGTH) {

            if (world->cubes[intPosition.x][intPosition.y][intPosition.z] != 0) {
                // Cube found

                camera->pointedCube = intPosition;
                camera->isPointingToCube = 1;
                cubeNotFound = 0;
            }
            else {
                nearCube = intPosition;
            }
        }
        i++;
    }

    if (cubeNotFound) {
        camera->isPointingToCube = 0;
    }
    else {
        camera->nearPointedCube = nearCube;
    }
}
