#include "linear-algebra.h"
#include <math.h>

/* OPERATION */

HxfMat4 hxfMat4MulMat(HxfMat4 a, HxfMat4 b) {
    return (HxfMat4){{
        {
            a.mat[0][0] * b.mat[0][0] + a.mat[0][1] * b.mat[1][0] + a.mat[0][2] * b.mat[2][0] + a.mat[0][3] * b.mat[3][0],
            a.mat[0][0] * b.mat[0][1] + a.mat[0][1] * b.mat[1][1] + a.mat[0][2] * b.mat[2][1] + a.mat[0][3] * b.mat[3][1],
            a.mat[0][0] * b.mat[0][2] + a.mat[0][1] * b.mat[1][2] + a.mat[0][2] * b.mat[2][2] + a.mat[0][3] * b.mat[3][2],
            a.mat[0][0] * b.mat[0][3] + a.mat[0][1] * b.mat[1][3] + a.mat[0][2] * b.mat[2][3] + a.mat[0][3] * b.mat[3][3],
        },
        {
            a.mat[1][0] * b.mat[0][0] + a.mat[1][1] * b.mat[1][0] + a.mat[1][2] * b.mat[2][0] + a.mat[1][3] * b.mat[3][0],
            a.mat[1][0] * b.mat[0][1] + a.mat[1][1] * b.mat[1][1] + a.mat[1][2] * b.mat[2][1] + a.mat[1][3] * b.mat[3][1],
            a.mat[1][0] * b.mat[0][2] + a.mat[1][1] * b.mat[1][2] + a.mat[1][2] * b.mat[2][2] + a.mat[1][3] * b.mat[3][2],
            a.mat[1][0] * b.mat[0][3] + a.mat[1][1] * b.mat[1][3] + a.mat[1][2] * b.mat[2][3] + a.mat[1][3] * b.mat[3][3],
        },
        {
            a.mat[2][0] * b.mat[0][0] + a.mat[2][1] * b.mat[1][0] + a.mat[2][2] * b.mat[2][0] + a.mat[2][3] * b.mat[3][0],
            a.mat[2][0] * b.mat[0][1] + a.mat[2][1] * b.mat[1][1] + a.mat[2][2] * b.mat[2][1] + a.mat[2][3] * b.mat[3][1],
            a.mat[2][0] * b.mat[0][2] + a.mat[2][1] * b.mat[1][2] + a.mat[2][2] * b.mat[2][2] + a.mat[2][3] * b.mat[3][2],
            a.mat[2][0] * b.mat[0][3] + a.mat[2][1] * b.mat[1][3] + a.mat[2][2] * b.mat[2][3] + a.mat[2][3] * b.mat[3][3],
        },
        {
            a.mat[3][0] * b.mat[0][0] + a.mat[3][1] * b.mat[1][0] + a.mat[3][2] * b.mat[2][0] + a.mat[3][3] * b.mat[3][0],
            a.mat[3][0] * b.mat[0][1] + a.mat[3][1] * b.mat[1][1] + a.mat[3][2] * b.mat[2][1] + a.mat[3][3] * b.mat[3][1],
            a.mat[3][0] * b.mat[0][2] + a.mat[3][1] * b.mat[1][2] + a.mat[3][2] * b.mat[2][2] + a.mat[3][3] * b.mat[3][2],
            a.mat[3][0] * b.mat[0][3] + a.mat[3][1] * b.mat[1][3] + a.mat[3][2] * b.mat[2][3] + a.mat[3][3] * b.mat[3][3],
        }
        
    }};
}

HxfVec4 hxfMat4MulVec(HxfMat4 a, HxfVec4 b) {
    return (HxfVec4){
        a.mat[0][0] * b.x + a.mat[0][1] * b.y + a.mat[0][2] * b.z + a.mat[0][3] * b.w,
        a.mat[1][0] * b.x + a.mat[1][1] * b.y + a.mat[1][2] * b.z + a.mat[1][3] * b.w,
        a.mat[2][0] * b.x + a.mat[2][1] * b.y + a.mat[2][2] * b.z + a.mat[2][3] * b.w,
        a.mat[3][0] * b.x + a.mat[3][1] * b.y + a.mat[3][2] * b.z + a.mat[3][3] * b.w
    };
}

HxfVec3 hxfVec3Cross(HxfVec3 a, HxfVec3 b) {
    return (HxfVec3) {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float hxfVec3Dot(HxfVec3 a, HxfVec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

HxfVec3 hxfVec3Normalize(HxfVec3 a) {
    const float div = sqrtf(powf(a.x, 2) + powf(a.y, 2) + powf(a.z, 2));
    a.x /= div;
    a.y /= div;
    a.z /= div;

    return a;
}

/* TRANSFORMATION */

HxfMat4 hxfMat4ScaleMatrix(HxfVec3 factor) {
    HxfMat4 res = HXF_MAT4_IDENTITY;
    res.mat[0][0] *= factor.x;
    res.mat[1][1] *= factor.y;
    res.mat[2][2] *= factor.z;
    return res;
}

HxfMat4 hxfMat4TranslationMatrix(HxfVec3 factor) {
    HxfMat4 res = HXF_MAT4_IDENTITY;
    res.mat[3][0] = factor.x;
    res.mat[3][1] = factor.y;
    res.mat[3][2] = factor.z;

    return res;
}

HxfMat4 hxfMat4Rotate(float angle, HxfVec3 position) {
    float cos = cosf(angle);
    float sin = sinf(angle);

    return (HxfMat4){{
        {
            cos + powf(position.x, 2) * (1 - cos),
            position.x * position.y * (1 - cos) - position.z * sin,
            position.x * position.z * (1 - cos) + position.y * sin,
            0
        },
        {
            position.y * position.x * (1 - cos) + position.z * sin,
            cos + powf(position.y, 2) * (1 - cos),
            position.y * position.z * (1 - cos) - position.x * sin,
            0
        },
        {
            position.z * position.x * (1 - cos) - position.y * sin,
            position.z * position.y * (1 - cos) + position.x * sin,
            cos + powf(position.z, 2) * (1 - cos),
            0
        },
        {
            0,
            0,
            0,
            1
        }
    }};
}

HxfMat4 hxfPerspectiveProjection(float near, float far, float fov, float aspect) {
    // const float tan = tanf(fov / 2.0f);

    // return (HxfMat4){{
    //     {1.0f / (aspect * tan), 0.0f, 0.0f, 0.0f},
    //     {0.0f, 1.0f / tan, 0.0f, 0.0f},
    //     {0.0f, 0.0f, far / (far - near), 1.0f},
    //     {0.0f, 0.0f, 0.0f, - (far * near) / (far - near)}
    // }};

    const float tanHalfFovy = tan(fov / 2.f);
    HxfMat4 projectionMatrix = {0};
    projectionMatrix.mat[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix.mat[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix.mat[2][2] = far / (far - near);
    projectionMatrix.mat[2][3] = 1.f;
    projectionMatrix.mat[3][2] = -(far * near) / (far - near);
    return projectionMatrix;
}

HxfMat4 hxfViewMatrix(HxfVec3 position, HxfVec3 direction, HxfVec3 up) {
    HxfVec3 w = hxfVec3Normalize(direction);
    HxfVec3 u = hxfVec3Normalize(hxfVec3Cross(w, up));
    HxfVec3 v = hxfVec3Cross(w, u);

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
    res.mat[3][0] = -hxfVec3Dot(u, position);
    res.mat[3][1] = -hxfVec3Dot(v, position);
    res.mat[3][2] = -hxfVec3Dot(w, position);

    return res;
}