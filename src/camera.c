#include "camera.h"
#include <math.h>

void hxfUpdatePointedCube(HxfCamera* restrict camera, const HxfWorld* restrict world) {
    const int maxPointingDistance = 5; // Maximum distance at which the block can be selected
    const int precision = 50;

    int i = 1;
    int cubeNotFound = 1;

    const HxfVec3 forward = { camera->direction.x / (float)precision, camera->direction.y / (float)precision, camera->direction.z / (float)precision };

    HxfVec3 floatPosition = camera->position;
    HxfIvec3 nearCube = { 0 };
    int nearCubeNotSet = 1;

    while (cubeNotFound && i != maxPointingDistance * precision) {
        // Select the next cube

        floatPosition = hxfVec3Add(&floatPosition, &forward);
        HxfIvec3 intPosition = roundVector(&floatPosition);

        const HxfIvec3 worldPiecePosition = hxfWorldGetPiecePositionF(&floatPosition);
        HxfMapElement* const worldPieceElement = hxfMapGet(&world->pieces, &worldPiecePosition);

        if (worldPieceElement != NULL) {
            HxfWorldPiece* const worldPiece = (HxfWorldPiece*)worldPieceElement->value;

            const HxfIvec3 worldPieceRelativePosition = hxfWorldGetLocalPosition(&intPosition);
            if (worldPiece->cubes[worldPieceRelativePosition.x][worldPieceRelativePosition.y][worldPieceRelativePosition.z] != 0) {
                camera->pointedCube = intPosition;
                camera->isPointingToCube = 1;
                cubeNotFound = 0;
            }
            else {
                nearCube = intPosition;
                nearCubeNotSet = 0;
            }
        }
        else {
            nearCube = intPosition;
            nearCubeNotSet = 0;
        }

        i++;
    }

    if (cubeNotFound || nearCubeNotSet) {
        camera->isPointingToCube = 0;
    }
    else {
        camera->nearPointedCube = nearCube;
    }
}
