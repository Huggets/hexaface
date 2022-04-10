#include "linear-algebra.h"
#include <math.h>

/* OPERATION */

HxfMat4 hxfMat4MulMat(const HxfMat4* restrict a, const HxfMat4* restrict b) {
    const HxfMat4 multiplicationMatrix = {
        {
            {
                a->mat[0][0] * b->mat[0][0] + a->mat[0][1] * b->mat[1][0] + a->mat[0][2] * b->mat[2][0] + a->mat[0][3] * b->mat[3][0],
                    a->mat[0][0] * b->mat[0][1] + a->mat[0][1] * b->mat[1][1] + a->mat[0][2] * b->mat[2][1] + a->mat[0][3] * b->mat[3][1],
                    a->mat[0][0] * b->mat[0][2] + a->mat[0][1] * b->mat[1][2] + a->mat[0][2] * b->mat[2][2] + a->mat[0][3] * b->mat[3][2],
                    a->mat[0][0] * b->mat[0][3] + a->mat[0][1] * b->mat[1][3] + a->mat[0][2] * b->mat[2][3] + a->mat[0][3] * b->mat[3][3],
            },
            {
                a->mat[1][0] * b->mat[0][0] + a->mat[1][1] * b->mat[1][0] + a->mat[1][2] * b->mat[2][0] + a->mat[1][3] * b->mat[3][0],
                    a->mat[1][0] * b->mat[0][1] + a->mat[1][1] * b->mat[1][1] + a->mat[1][2] * b->mat[2][1] + a->mat[1][3] * b->mat[3][1],
                    a->mat[1][0] * b->mat[0][2] + a->mat[1][1] * b->mat[1][2] + a->mat[1][2] * b->mat[2][2] + a->mat[1][3] * b->mat[3][2],
                    a->mat[1][0] * b->mat[0][3] + a->mat[1][1] * b->mat[1][3] + a->mat[1][2] * b->mat[2][3] + a->mat[1][3] * b->mat[3][3],
            },
            {
                a->mat[2][0] * b->mat[0][0] + a->mat[2][1] * b->mat[1][0] + a->mat[2][2] * b->mat[2][0] + a->mat[2][3] * b->mat[3][0],
                    a->mat[2][0] * b->mat[0][1] + a->mat[2][1] * b->mat[1][1] + a->mat[2][2] * b->mat[2][1] + a->mat[2][3] * b->mat[3][1],
                    a->mat[2][0] * b->mat[0][2] + a->mat[2][1] * b->mat[1][2] + a->mat[2][2] * b->mat[2][2] + a->mat[2][3] * b->mat[3][2],
                    a->mat[2][0] * b->mat[0][3] + a->mat[2][1] * b->mat[1][3] + a->mat[2][2] * b->mat[2][3] + a->mat[2][3] * b->mat[3][3],
            },
            {
                a->mat[3][0] * b->mat[0][0] + a->mat[3][1] * b->mat[1][0] + a->mat[3][2] * b->mat[2][0] + a->mat[3][3] * b->mat[3][0],
                    a->mat[3][0] * b->mat[0][1] + a->mat[3][1] * b->mat[1][1] + a->mat[3][2] * b->mat[2][1] + a->mat[3][3] * b->mat[3][1],
                    a->mat[3][0] * b->mat[0][2] + a->mat[3][1] * b->mat[1][2] + a->mat[3][2] * b->mat[2][2] + a->mat[3][3] * b->mat[3][2],
                    a->mat[3][0] * b->mat[0][3] + a->mat[3][1] * b->mat[1][3] + a->mat[3][2] * b->mat[2][3] + a->mat[3][3] * b->mat[3][3],
            }
        }
    };

    return multiplicationMatrix;
}

HxfVec4 hxfMat4MulVec(const HxfMat4* restrict a, const HxfVec4* restrict b) {
    const HxfVec4 multiplicationVector = {
        a->mat[0][0] * b->x + a->mat[0][1] * b->y + a->mat[0][2] * b->z + a->mat[0][3] * b->w,
            a->mat[1][0] * b->x + a->mat[1][1] * b->y + a->mat[1][2] * b->z + a->mat[1][3] * b->w,
            a->mat[2][0] * b->x + a->mat[2][1] * b->y + a->mat[2][2] * b->z + a->mat[2][3] * b->w,
            a->mat[3][0] * b->x + a->mat[3][1] * b->y + a->mat[3][2] * b->z + a->mat[3][3] * b->w
    };

    return multiplicationVector;
}

HxfVec3 hxfVec3Cross(const HxfVec3* restrict a, const HxfVec3* restrict b) {
    const HxfVec3 crossProduct = {
        a->y * b->z - a->z * b->y,
            a->z * b->x - a->x * b->z,
            a->x * b->y - a->y * b->x
    };

    return crossProduct;
}

float hxfVec3Dot(const HxfVec3* restrict a, const HxfVec3* restrict b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

HxfVec3 hxfVec3Normalize(const HxfVec3* restrict a) {
    const float div = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z); // sqrt(a**2 + b**2 + c**2)

    const HxfVec3 normalizedVector = {
        a->x / div,
        a->y / div,
        a->z / div
    };

    return normalizedVector;
}

HxfVec3 hxfVec3Add(const HxfVec3* restrict a, const HxfVec3* restrict b) {
    const HxfVec3 additionVector = {
        a->x + b->x,
        a->y + b->y,
        a->z + b->z
    };

    return additionVector;
}

HxfVec3 hxfVec3Sub(const HxfVec3* restrict a, const HxfVec3* restrict b) {
    const HxfVec3 substractionVector = {
        a->x - b->x,
        a->y - b->y,
        a->z - b->z
    };

    return substractionVector;
}

HxfIvec3 roundVector(HxfVec3* restrict vec) {
    HxfIvec3 roundedVector;

    if (vec->x < 0) {
        roundedVector.x = (int)floorf(vec->x);
    }
    else {
        roundedVector.x = (int)vec->x;
    }
    if (vec->y < 0) {
        roundedVector.y = (int)floorf(vec->y);
    }
    else {
        roundedVector.y = (int)vec->y;
    }
    if (vec->z < 0) {
        roundedVector.z = (int)floorf(vec->z);
    }
    else {
        roundedVector.z = (int)vec->z;
    }

    return roundedVector;
}

/* TRANSFORMATION */

HxfMat4 hxfMat4ScaleMatrix(const HxfVec3* restrict factor) {
    const HxfMat4 scaleMatrix = { {
        { factor->x, 0.0f, 0.0f, 0.0f },
        { 0.0f, factor->y, 0.0f, 0.0f },
        { 0.0f, 0.0f, factor->z, 0.0f },
        { 0.0, 0.0, 0.0f, 1.0f }
    } };

    return scaleMatrix;
}

HxfMat4 hxfMat4TranslationMatrix(const HxfVec3* restrict factor) {
    const HxfMat4 translationMatrix = { {
        { 1, 0, 0, factor->x },
        { 0, 1, 0, factor->y },
        { 0, 0, 1, factor->z },
        { 0, 0, 0, 1 }
    } };

    return translationMatrix;
}

HxfMat4 hxfMat4RotationMatrix(float angle, const HxfVec3* restrict axis) {
    const float cos = cosf(angle);
    const float sin = sinf(angle);

    const float xSin = axis->x * sin;
    const float ySin = axis->y * sin;
    const float zSin = axis->z * sin;
    const float oneMinusCos = 1 - cos;
    const float xy = axis->x * axis->y;
    const float xz = axis->x * axis->z;
    const float yz = axis->y * axis->z;

    const HxfMat4 rotationMatrix = { {
        {
            cos + axis->x * axis->x * oneMinusCos,
                xy * oneMinusCos - zSin,
                xz * oneMinusCos + ySin,
                0.0f
        },
        {
            xy * oneMinusCos + zSin,
                cos + axis->y * axis->y * oneMinusCos,
                yz * oneMinusCos - xSin,
                0.0f
        },
        {
            xz * oneMinusCos - ySin,
                yz * oneMinusCos + xSin,
                cos + axis->z * axis->z * oneMinusCos,
                0.0f
        },
        {
            0.0f,
                0.0f,
                0.0f,
                1.0f
        }
    } };

    return rotationMatrix;
}

HxfMat4 hxfPerspectiveProjectionMatrix(float near, float far, float fov, float aspect) {
    const float tan = tanf(fov / 2.f);
    const float farMinusNear = far - near;

    const HxfMat4 projectionMatrix = { {
        { 1.0f / (aspect * tan), 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f / tan, 0.0f, 0.0f },
        { 0.0f, 0.0f, far / farMinusNear, 1.0f },
        { 0.0f, 0.0f, (-far * near) / farMinusNear, 0.0f }
    } };

    return projectionMatrix;
}

HxfMat4 hxfViewMatrix(const HxfVec3* restrict position, const HxfVec3* restrict forwardDirection, const HxfVec3* restrict upDirection) {
    const HxfVec3 right = hxfVec3Cross(forwardDirection, upDirection);
    const HxfVec3 normalizedRight = hxfVec3Normalize(&right);
    const HxfVec3 up = hxfVec3Cross(forwardDirection, &normalizedRight);

    const HxfMat4 viewMatrix = { {
        { normalizedRight.x, up.x, forwardDirection->x, 0.0f },
        { normalizedRight.y, up.y, forwardDirection->y, 0.0f },
        { normalizedRight.z, up.z, forwardDirection->z, 0.0f },
        { -hxfVec3Dot(&normalizedRight, position), -hxfVec3Dot(&up, position), -hxfVec3Dot(forwardDirection, position), 1.0f }
    } };

    return viewMatrix;
}