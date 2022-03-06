#include "algebra-linear.h"
#include <math.h>

HxfMat4 hxfMat4Identity() {
    HxfMat4 mat = {{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    return mat;
}

/* OPERATION */

HxfMat4 hxfMat4MulMat(HxfMat4 * a, HxfMat4 * b) {
    return (HxfMat4){{
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
        
    }};
}

HxfVec4 hxfMat4MulVec(HxfMat4 * a, HxfVec4 * b) {
    return (HxfVec4){
        a->mat[0][0] * b->x + a->mat[0][1] * b->y + a->mat[0][2] * b->z + a->mat[0][3] * b->w,
        a->mat[1][0] * b->x + a->mat[1][1] * b->y + a->mat[1][2] * b->z + a->mat[1][3] * b->w,
        a->mat[2][0] * b->x + a->mat[2][1] * b->y + a->mat[2][2] * b->z + a->mat[2][3] * b->w,
        a->mat[3][0] * b->x + a->mat[3][1] * b->y + a->mat[3][2] * b->z + a->mat[3][3] * b->w
    };
}

/* TRANSFORMATION */

void hxfMat4Scale(HxfMat4 * mat4, HxfVec3 * factor) {
    mat4->mat[0][0] *= factor->x;
    mat4->mat[1][1] *= factor->y;
    mat4->mat[2][2] *= factor->z;
}

void hxfMat4Translate(HxfMat4 * mat4, HxfVec3 * factor) {
    mat4->mat[0][3] = factor->x;
    mat4->mat[1][3] = factor->y;
    mat4->mat[2][3] = factor->z;
}

HxfMat4 hxfMat4Rotate(HxfMat4 * mat4, float angle, HxfVec3 * position) {
    float cos = cosf(angle);
    float sin = sinf(angle);

    return (HxfMat4){{
        {
            cos + powf(position->x, 2) * (1 - cos),
            position->x * position->y * (1 - cos) - position->z * sin,
            position->x * position->z * (1 - cos) + position->y * sin,
            0
        },
        {
            position->y * position->x * (1 - cos) + position->z * sin,
            cos + powf(position->y, 2) * (1 - cos),
            position->y * position->z * (1 - cos) - position->x * sin,
            0
        },
        {
            position->z * position->x * (1 - cos) - position->y * sin,
            position->z * position->y * (1 - cos) + position->x * sin,
            cos + powf(position->z, 2) * (1 - cos),
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
    const float tan = tanf(fov / 2.0f);

    return (HxfMat4){{
        {1.0f / (aspect * tan), 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f / tan, 0.0f, 0.0f},
        {0.0f, 0.0f, far / (far + near), 1.0f},
        {0.0f, 0.0f, 0.0f, - (far * near) / (far - near)}
    }};
}