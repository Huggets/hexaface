#include "linear-algebra.h"
#include <math.h>

/* OPERATION */

HxfMat4 hxfMat4MulMat(const HxfMat4* restrict a, const HxfMat4* restrict b) {
    return (HxfMat4) {
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
}

HxfVec4 hxfMat4MulVec(const HxfMat4* restrict a, const HxfVec4* restrict b) {
    return (HxfVec4) {
        a->mat[0][0] * b->x + a->mat[0][1] * b->y + a->mat[0][2] * b->z + a->mat[0][3] * b->w,
            a->mat[1][0] * b->x + a->mat[1][1] * b->y + a->mat[1][2] * b->z + a->mat[1][3] * b->w,
            a->mat[2][0] * b->x + a->mat[2][1] * b->y + a->mat[2][2] * b->z + a->mat[2][3] * b->w,
            a->mat[3][0] * b->x + a->mat[3][1] * b->y + a->mat[3][2] * b->z + a->mat[3][3] * b->w
    };
}

HxfVec3 hxfVec3Cross(const HxfVec3* restrict a, const HxfVec3* restrict b) {
    return (HxfVec3) {
        a->y* b->z - a->z * b->y,
            a->z* b->x - a->x * b->z,
            a->x* b->y - a->y * b->x
    };
}

float hxfVec3Dot(const HxfVec3* restrict a, const HxfVec3* restrict b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

HxfVec3 hxfVec3Normalize(const HxfVec3* restrict a) {
    HxfVec3 ret = *a;
    const float div = sqrtf(powf(a->x, 2) + powf(a->y, 2) + powf(a->z, 2)); // sqrt(a**2 + b**2 + c**2)
    ret.x /= div;
    ret.y /= div;
    ret.z /= div;

    return ret;
}

HxfVec3 hxfVec3Add(const HxfVec3* restrict a, const HxfVec3* restrict b) {
    HxfVec3 ret = *a;
    ret.x += b->x;
    ret.y += b->y;
    ret.z += b->z;
    return ret;
}

HxfVec3 hxfVec3Sub(const HxfVec3* restrict a, const HxfVec3* restrict b) {
    HxfVec3 ret = *a;
    ret.x -= b->x;
    ret.y -= b->y;
    ret.z -= b->z;
    return ret;
}


/* TRANSFORMATION */

HxfMat4 hxfMat4ScaleMatrix(const HxfVec3* restrict factor) {
    HxfMat4 res = HXF_MAT4_IDENTITY;
    res.mat[0][0] *= factor->x;
    res.mat[1][1] *= factor->y;
    res.mat[2][2] *= factor->z;
    return res;
}

HxfMat4 hxfMat4TranslationMatrix(const HxfVec3* restrict factor) {
    HxfMat4 res = HXF_MAT4_IDENTITY;
    res.mat[3][0] = factor->x;
    res.mat[3][1] = factor->y;
    res.mat[3][2] = factor->z;

    return res;
}

HxfMat4 hxfMat4RotationMatrix(float angle, const HxfVec3* restrict axis) {
    float cos = cosf(angle);
    float sin = sinf(angle);

    return (HxfMat4) {
        {
            {
                cos + powf(axis->x, 2) * (1 - cos),
                    axis->x* axis->y* (1 - cos) - axis->z * sin,
                    axis->x* axis->z* (1 - cos) + axis->y * sin,
                    0
            },
            {
                axis->y * axis->x * (1 - cos) + axis->z * sin,
                    cos + powf(axis->y, 2) * (1 - cos),
                    axis->y * axis->z * (1 - cos) - axis->x * sin,
                    0
            },
            {
                axis->z * axis->x * (1 - cos) - axis->y * sin,
                    axis->z * axis->y * (1 - cos) + axis->x * sin,
                    cos + powf(axis->z, 2) * (1 - cos),
                    0
            },
            {
                0,
                    0,
                    0,
                    1
            }
        }
    };
}

HxfMat4 hxfPerspectiveProjection(float near, float far, float fov, float aspect) {
    const float tanHalfFovy = tan(fov / 2.f);
    HxfMat4 projectionMatrix = { 0 };
    projectionMatrix.mat[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix.mat[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix.mat[2][2] = far / (far - near);
    projectionMatrix.mat[2][3] = 1.f;
    projectionMatrix.mat[3][2] = -(far * near) / (far - near);
    return projectionMatrix;
}

HxfMat4 hxfViewMatrix(const HxfVec3* restrict position, const HxfVec3* restrict direction, const HxfVec3* restrict up) {
    HxfVec3 w = hxfVec3Normalize(direction);
    HxfVec3 cross = hxfVec3Cross(&w, up);
    HxfVec3 u = hxfVec3Normalize(&cross);
    HxfVec3 v = hxfVec3Cross(&w, &u);

    HxfMat4 res = HXF_MAT4_IDENTITY;
    res.mat[0][0] = u.x;
    res.mat[1][0] = u.y;
    res.mat[2][0] = u.z;
    res.mat[0][1] = v.x;
    res.mat[1][1] = v.y;
    res.mat[2][1] = v.z;
    res.mat[0][2] = w.x;
    res.mat[1][2] = w.y;
    res.mat[2][2] = w.z;
    res.mat[3][0] = -hxfVec3Dot(&u, position);
    res.mat[3][1] = -hxfVec3Dot(&v, position);
    res.mat[3][2] = -hxfVec3Dot(&w, position);

    return res;
}